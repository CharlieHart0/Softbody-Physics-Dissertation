using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using SimpleJSON;
using System;
using System.IO;
using System.Text;

// a script to export part of a unity scene into a JSON file, which can be imported into Team 4's game engine
public class Team4MapToJSON : MonoBehaviour
{
    [Header("Map Settings")]
    [SerializeField] string mapName;
    [SerializeField] bool overwriteExistingFile = false;
    [SerializeField] bool overwriteExistingMapObject = false;
    [SerializeField] GameObject mapObject;

    // Start is called before the first frame update
    void Start()
    {
        

    }

    [EasyButtons.Button]
    void ExportMap()
    {
        Debug.Log("Exporting Map");

        if (mapObject.transform.childCount == 0)
        {
            Debug.LogWarning("Map Object has no children, are you sure you arent doing something wrong?");
            return;
        }
        JSONObject jObj = gameObjectToJOBJ(mapObject);

        string filepath = Application.dataPath +"/"+ mapName + ".json";

        if(File.Exists(filepath) && !overwriteExistingFile)
        {
            Debug.LogError("File already exists at "+filepath+" - and set not to overwrite!");
            return;
        }

        StringBuilder sb = new();

        jObj.WriteToStringBuilder(sb, 3, 6, JSONTextMode.Indent);

        StreamWriter sw = new StreamWriter(filepath);

        sw.Write(sb.ToString());

        sw.Close();

        Debug.Log("Map Exported!");
    }

    [EasyButtons.Button]
    void ImportMap()
    {
        Debug.Log("Importing Map");

        if(!overwriteExistingMapObject && mapObject.transform.childCount != 0)
        {
            Debug.LogWarning("Attempted to overwrite existing map object with children, but setting disallows this.");
            return;
        }

       
        int childCount = mapObject.transform.childCount;
        for (int i = 0; i < childCount ; i++)
        {
            DestroyImmediate(mapObject.transform.GetChild(0).gameObject);
        }

        string filepath = Application.dataPath + "/" + mapName + ".json";
        JSONObject jObj = new();

        using (StreamReader file = new StreamReader(filepath))
        {
            jObj = (JSONObject)JSONObject.Parse(file.ReadToEnd());
        }

        mapObject.name = jObj["name"].Value;
        JOBJtoTransform(jObj["transform"], mapObject.transform);

        foreach(JSONNode jnode in jObj["children"].AsArray)
        {
            JOBJtoGameObject(jnode, mapObject.transform);
        }

      

       
        Debug.Log("Map Imported!");
    }

    [Header("Mesh References")]
    [SerializeField] Material defaultMaterial;
    [SerializeField] Material pinkMaterial;
    [SerializeField] Material blueMaterial;
    [SerializeField] Mesh sphereMesh;
    [SerializeField] Mesh cubeMesh;
    [SerializeField] Mesh rhinoMesh;

    #region jobj to something else

    Vector3 JOBJtoVec3(JSONNode jObj)
    {
        Vector3 vec = Vector3.zero;
        vec.x = jObj["x"].AsFloat;
        vec.y = jObj["y"].AsFloat;
        vec.z = jObj["z"].AsFloat;
        return vec;
    }

    //does not include children
    void JOBJtoTransform(JSONNode jObj, Transform t)
    {
        t.localPosition = v3flipX(JOBJtoVec3(jObj["l_position"]));
        t.localRotation = Quaternion.Euler(JOBJtoVec3(jObj["l_rotation"]));
        t.localScale = JOBJtoVec3(jObj["l_scale"]);
    }

    private void JOBJtoGameObject(JSONNode jnode, Transform parent)
    {
        GameObject obj = new(jnode["name"].Value);
        obj.transform.SetParent(parent);
        JOBJtoTransform(jnode["transform"], obj.transform);

        if(jnode["tag"].Value != "")
        {
            obj.tag = jnode["tag"].Value;
        }

        List<string> tags = new();

        if (jnode.HasKey("tags"))
        {
            JSONArray arr = jnode["tags"].AsArray;
            foreach(JSONNode s in arr)
            {
                tags.Add(s.Value.ToString());
            }

            obj.AddComponent<Team4ObjectTags>().mapObjectTags = tags;
        }


        // do other components
        if(jnode["mesh"].Value != "") { JNodeToMeshRenderer(jnode, obj); }
        if (jnode.HasKey("physicsObject")) { JNodeToRB(jnode, obj); }
        if (jnode.HasKey("collider")) { JNodeToCollider(jnode, obj); }


        //if rhino

        if (tags.Contains("rhino"))
        {
            BoxCollider bcol = obj.AddComponent<BoxCollider>();
            bcol.center = v3flipX(JOBJtoVec3(jnode["rhinoBox"]["centre world"])) - obj.transform.position;
            bcol.size = JOBJtoVec3(jnode["rhinoBox"]["fullDim"]);
        }


        // if trigger box

        if (tags.Contains("respawntrigger") || tags.Contains("checkpoint"))
        {
            Vector3 scale = JOBJtoVec3(jnode["collider"]["fullDimensions"]);

            scale.x /= obj.transform.parent.lossyScale.x;
            scale.y /= obj.transform.parent.lossyScale.y;
            scale.z /= obj.transform.parent.lossyScale.z;

            obj.transform.localScale = scale;
            obj.GetComponent<BoxCollider>().size = new Vector3(1, 1, 1);

            if (tags.Contains("respawntrigger"))
            {
                obj.GetComponent<MeshRenderer>().material = pinkMaterial;
            }
            else if (tags.Contains("checkpoint"))
            {
                obj.GetComponent<MeshRenderer>().material = blueMaterial;
            }
        }


        // instantiate children
        if (jnode["children"].IsArray)
        {
            foreach(JSONNode child in jnode["children"].AsArray)
            {
                JOBJtoGameObject(child, obj.transform);
            }
        }
    }

    void JNodeToCollider(JSONNode jnode, GameObject obj)
    {
        switch (jnode["collider"]["type"].Value)
        {
            // TODO implement other collider types
            case "box":
                BoxCollider bcol = obj.AddComponent<BoxCollider>();
                bcol.center = v3flipX(JOBJtoVec3(jnode["collider"]["centre"]));
                Vector3 scale = JOBJtoVec3(jnode["collider"]["fullDimensions"]);
                Debug.Log(scale);
                scale.x /= obj.transform.localScale.x;
                scale.y /= obj.transform.localScale.y;
                scale.z /= obj.transform.localScale.z;
                Debug.Log(scale);
                bcol.size = scale;
                            
                break;

            case "sphere":
                SphereCollider scol = obj.AddComponent<SphereCollider>();
                scol.center = v3flipX(JOBJtoVec3(jnode["collider"]["centre"]));
                scol.radius = jnode["collider"]["radius"].AsFloat;
                break;
            default:
                Debug.LogError("Invalid collider type\"" + jnode["collider"]["type"].Value + "\" imported!");
                return;
        }
    }

    void JNodeToRB(JSONNode jNode, GameObject obj)
    {
        Rigidbody rb = obj.AddComponent<Rigidbody>();
        if(jNode["physicsObject"]["inverseMass"].Value == "INF")
        {
            rb.mass = 0;
        }
        else if(jNode["physicsObject"]["inverseMass"].AsFloat == 0)
        {
            rb.mass = 999999;
        }
        else
        {
            rb.mass = 1 / jNode["physicsObject"]["inverseMass"].AsFloat;
        }

        rb.constraints = (RigidbodyConstraints)jNode["physicsObject"]["constraints"].AsInt;
    }

    void JNodeToMeshRenderer(JSONNode jNode, GameObject obj)
    {
        MeshFilter mf = obj.AddComponent<MeshFilter>();
        MeshRenderer mr = obj.AddComponent<MeshRenderer>();

        // TODO add custom materials
        mr.material = defaultMaterial;

        switch (jNode["mesh"].Value)
        {
            case "Sphere":
                mf.sharedMesh = sphereMesh;
                break;
            case "Cube":
                mf.sharedMesh = cubeMesh;
                break;
            case "rhino":
                mf.sharedMesh = rhinoMesh;
                break;

            default:
                mf.sharedMesh = null;
                Debug.LogError("Invalid mesh found in json :(");
                return;
        }
    }

    #endregion


    #region type to jobj
    JSONObject gameObjectToJOBJ(GameObject obj)
    {
        JSONObject jObj = new();

        jObj.Add("name", obj.name);
        jObj.Add("transform", transformToJOBJ(obj.transform));

        if(obj.transform.childCount != 0)
        {
            JSONArray children = new();
            for (int i = 0; i < obj.transform.childCount; ++i)
            {
                children.Add("", gameObjectToJOBJ(obj.transform.GetChild(i).gameObject));
            }
            jObj.Add("children", children);
        }

        List<string> tags = new();

        if (obj.GetComponent<Team4ObjectTags>()) { jObj.Add("tags", tagsToJOBJ(obj.GetComponent<Team4ObjectTags>()));

            tags = obj.GetComponent<Team4ObjectTags>().mapObjectTags;
        }

        

        if (obj.GetComponent<MeshFilter>()) { jObj.Add("mesh", meshToJOBJstr(obj)); }

        if (obj.GetComponent<Collider>() && !tags.Contains("rhino")) { jObj.Add("collider", colliderToJOBJ(obj.GetComponent<Collider>())); }

        if (obj.GetComponent<Rigidbody>()) { jObj.Add("physicsObject", rbToJOBJ(obj.GetComponent<Rigidbody>())); }

        if (tags.Contains("rhino")) { jObj.Add("rhinoBox", rhinoDetectorBoxToJOBJ(obj)); }
       

        if (obj.tag != "Untagged") { jObj.Add("tag", obj.tag); }

        return jObj;
    }

    JSONNode rhinoDetectorBoxToJOBJ(GameObject obj)
    {
        JSONObject jObj = new();

        if(obj.GetComponent<BoxCollider>() == null)
        {
            Debug.LogError("Rhino does not have player detector box :(");
            return null;
        }

        BoxCollider bc = obj.GetComponent<BoxCollider>();

        jObj.Add("centre world", vec3ToJOBJ(v3flipX(bc.center + obj.transform.position)));
        jObj.Add("fullDim", vec3ToJOBJ(bc.size));


        return jObj;

    }

    JSONArray tagsToJOBJ(Team4ObjectTags objectTags)
    {
        JSONArray jObj = new();
        int i = 0;
        foreach(string s in objectTags.mapObjectTags)
        {
            jObj[i] = s.ToLower();
            i++;
        }
        return jObj;
    }

    JSONObject rbToJOBJ(Rigidbody rigidbody)
    {
        JSONObject jObj = new();

        if(rigidbody.mass == 0)
        {
            jObj.Add("inverseMass", "INF");
        }
        else if(rigidbody.mass >= 999999)
        {
            jObj.Add("inverseMass", 0);
        }
        else
        {
            jObj.Add("inverseMass", 1 / rigidbody.mass);
        }

        jObj.Add("constraints", (int)rigidbody.constraints);

        return jObj;
    }

    JSONObject colliderToJOBJ(Collider col)
    {
        JSONObject jObj = new();

        //TODO implement compound colliders, other shapes.

        if(col.GetType() == typeof(BoxCollider)) // This will currently only be implemented as AABB in game :(
        {
            BoxCollider bcol = (BoxCollider)col;
            jObj.Add("type", "box");
            jObj.Add("centre", vec3ToJOBJ(v3flipX(bcol.center)));
            if(bcol.center != Vector3.zero) { Debug.LogWarning("Team4's game does not support offset bounding volumes at the moment :("); }
            jObj.Add("fullDimensions", vec3ToJOBJ(Vector3.Scale(bcol.size, col.gameObject.transform.lossyScale)));
        }
        else if (col.GetType() == typeof(SphereCollider))
        {
            SphereCollider scol = (SphereCollider)col;
            jObj.Add("type", "sphere");
            jObj.Add("centre", vec3ToJOBJ(v3flipX(scol.center)));
            if (scol.center != Vector3.zero) { Debug.LogWarning("Team4's game does not support offset bounding volumes at the moment :("); }
            jObj.Add("radius", scol.radius);
        }
        else
        {
            Debug.LogError("Invalid collider type on object " + col.gameObject.name);
        }

        return jObj;
    }

    JSONObject transformToJOBJ(Transform t, bool includePos = true, bool includeRot = true, bool includeScale = true)
    {

        JSONObject jObj = new();

        if (includePos) { jObj.Add("l_position", vec3ToJOBJ(v3flipX(t.localPosition))); }
        if (includePos) { jObj.Add("w_position", vec3ToJOBJ(v3flipX(t.position))); }
        if (includeRot) { jObj.Add("l_rotation", vec3ToJOBJ(t.localRotation.eulerAngles)); }
        if (includeScale) { jObj.Add("l_scale", vec3ToJOBJ(t.localScale)); }
        if (includeScale) { jObj.Add("w_scale", vec3ToJOBJ(t.lossyScale)); }


        return jObj;
    }

    JSONObject vec3ToJOBJ(Vector3 vec)
    {

        JSONObject jObj = new();

        jObj.Add("x", vec.x);
        jObj.Add("y", vec.y);
        jObj.Add("z", vec.z);

        return jObj;
    }

    string meshToJOBJstr(GameObject obj)
    {
        MeshFilter mf = obj.GetComponent<MeshFilter>();

        switch (mf.sharedMesh.name)
        {
            case "Cube":
            case "Cube Instance":
                return "Cube";

            case "Sphere":
            case "Sphere Instance":
                return "Sphere";

            case "Rhino":
            case "rhino_new.2":
                return "rhino";


            default:
                Debug.LogError("Object had mesh name \"" + mf.mesh.name + "\", but no corresponding mesh file was found. If this is unexpected, make sure to update Team4MapToJson.meshToJOBJstr accordingly.");
                return "ERROR";

        }
    }

    #endregion


    Vector3 v3flipX(Vector3 vec) { vec.x *= -1; return vec; }
}
