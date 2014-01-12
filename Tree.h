#include "Node.h"
#include <stdio.h>
#pragma once
class Tree{
		
		int delta;
        Node *root;
        // private methods
        Node* search(Node *m, int num);
        void insert(Node *&, int num);        // CALL BY REFERENCE.
        void inorder(Node *) const;
        void visit(Node *p) const {
			printf("%d\t", p->avarage);
        }
        void transplant(Node *u, Node *v) {
                Node *pa = parent(u);
                if(pa == 0)
                        root = v;
                else if (u == pa->left)
                        pa->left = v;
                else pa->right = v;
        }
        int minimum(Node *);
        int maximum(Node *);

// public methods
public:
        Tree() {
                root = 0;
				delta=10;
        }
        void inorder() {
                inorder(root);
        }
        void insert(int num){
                insert(root, num);
        }
        Node* search(int num){
                return search(root, num);
        }
        void del(Node* &);
        Node* parent(Node* node) const{
                Node *temp = root;
                while(temp->left!=node  &&  temp->right!=node) {
                        if(node->avarage < temp->avarage)
                                temp = temp->left;
                        else
                                temp = temp->right;
                }
                return temp;
        }
        int maximum(){
                return maximum(root);
        }
        int minimum() {
                return minimum(root);
        }
        int successor(int);
};

int Tree::successor(int num)
{
        Node *temp = search(num);
        if(temp->right) {
                return minimum(temp->right);
        }
        else{
                Node *y = parent(temp);
                while(y!=0  &&  temp == y->right) {
                        temp = y;
                        y = parent(temp);
                }
        return y->avarage;
        }
}

void Tree::inorder(Node *q) const
{
        if(q!=0) {
                inorder(q->left);
                printf("%d  ", q->avarage);
                inorder(q->right);
        }
}


void Tree::insert(Node *&q, int num) {
        Node *temp = q, *prev = q;
        while(temp != 0) {
                prev = temp;
                if(num < temp->avarage)
                        temp = temp->left;
                else
                        temp = temp->right;
        }

        if(!q) {
                q = new Node(num);
        }
        else{
                if(num < prev->avarage)
                        prev->left = new Node(num);
                else
                        prev->right = new Node(num);
        }
}


Node* Tree::search(Node *q, int num)
{
        while(q->avarage != num  &&  q!=0) {
                if(num < q->avarage)
                        q = q->left;
                else
                        q = q->right;
        }

        return q;
}


void Tree::del(Node* &q)
{

        if(!q->left)
                transplant(q, q->right);
        else if(!q->right)
                transplant(q, q->left);
        else{
                Node *succ = search( minimum(q->right) );
                if(parent(succ) != q) {
                        transplant(succ, succ->right);
                        succ->right = q->right;
                }
                transplant(q, succ);
                succ->left = q->left;     //transplant doesn't take care of left and right subtrees. It just establishes link with parent of one node to another.
        }
        delete q; q = 0;

}

int Tree::maximum(Node *node)
{
        Node *temp = node;
        while(temp->right!=0) {
                temp = temp->right;
        }
        return temp->avarage;
}

int Tree::minimum(Node *node)
{
        Node *temp = node;
        while(temp->left != 0) {
                temp = temp->left;
        }
        return temp->avarage;
}

