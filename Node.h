#pragma once
class Node{

public:
        int avarage;
		int nrOfMembers;
		double members[10000];
        Node *left;
        Node *right;
        /****/
        Node(int num, Node *l = 0, Node*r = 0) {
                avarage = num; left= l; right = r;
        }
};

