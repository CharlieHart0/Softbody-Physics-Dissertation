#include "NavigationGrid.h"
#include "Assets.h"

#include <fstream>
#include "../stb-master/stb_image.h"
#include "../CSC8503CoreClasses//Debug.h"

using namespace NCL;
using namespace CSC8503;

const int LEFT_NODE		= 0;
const int RIGHT_NODE	= 1;
const int TOP_NODE		= 2;
const int BOTTOM_NODE	= 3;

const char WALL_NODE	= 'x';
const char FLOOR_NODE	= '.';

NavigationGrid::NavigationGrid()	{
	nodeSize	= 0;
	gridWidth	= 0;
	gridHeight	= 0;
	allNodes	= nullptr;
}

NavigationGrid::NavigationGrid(const std::string&filename) : NavigationGrid() {


	/*												why was this added?? this code is very mischievous
	std::ofstream f(Assets::DATADIR + filename);
	f << "hello!" << std::endl;
	f.close();
	*/


	std::ifstream infile(Assets::DATADIR + filename);

	infile >> nodeSize;
	infile >> gridWidth;
	infile >> gridHeight;

	

	allNodes = new GridNode[gridWidth * gridHeight];

	for (int y = 0; y < gridHeight; ++y) {
		for (int x = 0; x < gridWidth; ++x) {
			GridNode&n = allNodes[(gridWidth * y) + x];
			char type = 0;
			infile >> type;
			n.type = type;
			n.position = Vector3((float)(x * nodeSize), 0, (float)(y * nodeSize));
		}
	}
	
	//now to build the connectivity between the nodes
	for (int y = 0; y < gridHeight; ++y) {
		for (int x = 0; x < gridWidth; ++x) {
			GridNode&n = allNodes[(gridWidth * y) + x];		

			if (y > 0) { //get the above node
				n.connected[0] = &allNodes[(gridWidth * (y - 1)) + x];
			}
			if (y < gridHeight - 1) { //get the below node
				n.connected[1] = &allNodes[(gridWidth * (y + 1)) + x];
			}
			if (x > 0) { //get left node
				n.connected[2] = &allNodes[(gridWidth * (y)) + (x - 1)];
			}
			if (x < gridWidth - 1) { //get right node
				n.connected[3] = &allNodes[(gridWidth * (y)) + (x + 1)];
			}
			for (int i = 0; i < 4; ++i) {
				if (n.connected[i]) {
					if (n.connected[i]->type == '.') {
						n.costs[i]		= 1;
					}
					if (n.connected[i]->type == 'x') {
						n.connected[i] = nullptr; //actually a wall, disconnect!
					}
				}
			}
		}	
	}
}


// i just needed to overload the constructor to allow it to load a nav grid from an image, randombool is not used
// IMPORTANT -- the loaded map is not nessasarily the same as the one loaded in tutorialgame.cpp, make sure you load the right one
NCL::CSC8503::NavigationGrid::NavigationGrid(const std::string& filename, bool randombool)
{

	Vector3 mapOrigin = Vector3(0, -17, 0);
	nodeSize = 4.0f;
	int bpp;
	std::string s = "../Assets/" + filename;
	uint8_t* rgb_image = stbi_load(s.c_str(), &gridWidth, &gridHeight, &bpp, 3);


	allNodes = new GridNode[gridWidth * gridHeight];

	for (int h = 0; h < gridHeight; h++) {
		for (int w = 0; w < gridWidth; w++) {
			int locationInt = (w + (h * gridWidth)) * 3;

			Vector3 col = Vector3(rgb_image[locationInt], rgb_image[locationInt + 1], rgb_image[locationInt + 2]);


			GridNode& n = allNodes[(gridWidth * h) + w];
			char type = 0;

			// . air x impassable

			// only impassable blocks are walls and doors
			if (col == Vector3(255, 255, 255)) {
				type = 'x';
			}
			else if (col == Vector3(255, 0, 0)) {
				type = 'x';
			}
			else {
				type = '.';
			}

			n.type = type;
			n.position = Vector3((float)(w * nodeSize), 0, (float)(h * nodeSize)) + mapOrigin; 

			if (n.type == '.') {
				airnodes.push_back(n.position);
			}
		}
	}


		//now to build the connectivity between the nodes
		for (int y = 0; y < gridHeight; ++y) {
			for (int x = 0; x < gridWidth; ++x) {
				GridNode& n = allNodes[(gridWidth * y) + x];

				if (y > 0) { //get the above node
					n.connected[0] = &allNodes[(gridWidth * (y - 1)) + x];
				}
				if (y < gridHeight - 1) { //get the below node
					n.connected[1] = &allNodes[(gridWidth * (y + 1)) + x];
				}
				if (x > 0) { //get left node
					n.connected[2] = &allNodes[(gridWidth * (y)) + (x - 1)];
				}
				if (x < gridWidth - 1) { //get right node
					n.connected[3] = &allNodes[(gridWidth * (y)) + (x + 1)];
				}
				for (int i = 0; i < 4; ++i) {
					if (n.connected[i]) {
						if (n.connected[i]->type == '.') {
							n.costs[i] = 1;
						}
						if (n.connected[i]->type == 'x') {
							n.connected[i] = nullptr; //actually a wall, disconnect!
						}
					}
				}
				
			}
		}

}

NavigationGrid::~NavigationGrid()	{
	delete[] allNodes;
}

bool NavigationGrid::FindPath(const Vector3& from, const Vector3& to, NavigationPath& outPath) {
	//need to work out which node 'from' sits in, and 'to' sits in
	int fromX = ((int)from.x / nodeSize);
	int fromZ = ((int)from.z / nodeSize);

	int toX = ((int)to.x / nodeSize);
	int toZ = ((int)to.z / nodeSize);

	if (fromX < 0 || fromX > gridWidth - 1 ||
		fromZ < 0 || fromZ > gridHeight - 1) {
		return false; //outside of map region!
	}

	if (toX < 0 || toX > gridWidth - 1 ||
		toZ < 0 || toZ > gridHeight - 1) {
		return false; //outside of map region!
	}

	GridNode* startNode = &allNodes[(fromZ * gridWidth) + fromX];
	GridNode* endNode	= &allNodes[(toZ * gridWidth) + toX];

	std::vector<GridNode*>  openList;
	std::vector<GridNode*>  closedList;

	openList.push_back(startNode);

	startNode->f = 0;
	startNode->g = 0;
	startNode->parent = nullptr;

	GridNode* currentBestNode = nullptr;

	while (!openList.empty()) {
		currentBestNode = RemoveBestNode(openList);

		if (currentBestNode == endNode) {			//we've found the path!
			GridNode* node = endNode;
			while (node != nullptr) {
				outPath.PushWaypoint(node->position);
				node = node->parent;
			}
			return true;
		}
		else {
			for (int i = 0; i < 4; ++i) {
				GridNode* neighbour = currentBestNode->connected[i];
				if (!neighbour) { //might not be connected...
					continue;
				}	
				bool inClosed	= NodeInList(neighbour, closedList);
				if (inClosed) {
					continue; //already discarded this neighbour...
				}

				float h = Heuristic(neighbour, endNode);				
				float g = currentBestNode->g + currentBestNode->costs[i];
				float f = h + g;

				bool inOpen		= NodeInList(neighbour, openList);

				if (!inOpen) { //first time we've seen this neighbour
					openList.emplace_back(neighbour);
				}
				if (!inOpen || f < neighbour->f) {//might be a better route to this neighbour
					neighbour->parent = currentBestNode;
					neighbour->f = f;
					neighbour->g = g;
				}
			}
			closedList.emplace_back(currentBestNode);
		}
	}
	return false; //open list emptied out with no path!
}

Vector3 NCL::CSC8503::NavigationGrid::randomAirNode()
{
	if (airnodes.size() < 1) { return Vector3(0, 0, 0); }
	return airnodes[rand() % airnodes.size()];
}

void NCL::CSC8503::NavigationGrid::DrawConnections()
{
	
	for (int i = 0; i < gridWidth * gridHeight; i++) {
		for (int x = 0; x < 4; x++) {
			if (allNodes[i].connected[x] != nullptr) {
				Debug::DrawLine(allNodes[i].position, allNodes[i].connected[x]->position,Vector4(1,0,0,1));
			}
		}
	}
}

bool NavigationGrid::NodeInList(GridNode* n, std::vector<GridNode*>& list) const {
	std::vector<GridNode*>::iterator i = std::find(list.begin(), list.end(), n);
	return i == list.end() ? false : true;
}

GridNode*  NavigationGrid::RemoveBestNode(std::vector<GridNode*>& list) const {
	std::vector<GridNode*>::iterator bestI = list.begin();

	GridNode* bestNode = *list.begin();

	for (auto i = list.begin(); i != list.end(); ++i) {
		if ((*i)->f < bestNode->f) {
			bestNode	= (*i);
			bestI		= i;
		}
	}
	list.erase(bestI);

	return bestNode;
}

float NavigationGrid::Heuristic(GridNode* hNode, GridNode* endNode) const {
	return (hNode->position - endNode->position).Length();
}