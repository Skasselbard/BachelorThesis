/****************************************************************************
  This file is part of LoLA.

  LoLA is free software: you can redistribute it and/or modify it under the
  terms of the GNU Affero General Public License as published by the Free
  Software Foundation, either version 3 of the License, or (at your option)
  any later version.

  LoLA is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
  more details.

  You should have received a copy of the GNU Affero General Public License
  along with LoLA. If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/

/*
\file
\author Markus
\status new

\brief routines for handling conflictcluster
*/

#include <Core/Dimensions.h>
#include <Core/Runtime.h>
#include <Net/UnionFind.h>
#include <Net/Net.h>

// init values
arrayindex_t UnionFind::clusterSize;
int UnionFind::rootNodeMax;

// a Debug function for Printing the Cluster
// LCOV_EXCL_START
void UnionFind::printCluster(){
	return;
    fprintf(stderr,"clustersize %d\n",UnionFind::clusterSize);
    bool cluster_root_printed = false;
    for(int root_node = 0;root_node<-rootNodeMax;root_node++){
        cluster_root_printed = false;
        for(arrayindex_t i=0;i<Net::Card[TR];i++)
        {
            if(UnionFind::find(i,TR)==root_node){
                if(!cluster_root_printed)
                {
                    fprintf(stderr,"Cluster root %d :\n",root_node);
                    cluster_root_printed = true;
                }
                fprintf(stderr,"Transition(%d): \t %s \t\t Cluster: %d \n",i,Net::Name[TR][i],root_node);
            }
        }
        for(arrayindex_t i=0;i<Net::Card[PL];i++)
        {
            if(UnionFind::find(i,PL)==root_node){
                if(!cluster_root_printed)
                {
                    fprintf(stderr,"Cluster root %d :\n",root_node);
                    cluster_root_printed = true;
                }
                fprintf(stderr,"Place(%d): \t %s \t\t  Cluster %d \n",i,Net::Name[PL][i],root_node);
            }
        }
    }
}
// LCOV_EXCL_STOP

/*!
 This function calculates conflictcluster (disjoint sets of places and transitons)
 \post the value of Net::UnionFind is allocated and set
*/
void UnionFind::calculateConflictCluster()
{
    //allocate memory for the structure
    Net::UnionFind = new uf_node_t[Net::Card[PL]+Net::Card[TR]];
    // cardinalities of places 
    const arrayindex_t cardPL = Net::Card[PL];
    // and transitions
    const arrayindex_t cardTR = Net::Card[TR];

    // pointer for saving which node set were already visited
    arrayindex_t visited_ptr = 0;
    // pointer to the next free element in the result array eg the union find
    // structure
    arrayindex_t union_find_ptr = 0;

    // boolean value for marking changes after loop iteration
    bool changes = false;

    //bool arrays to hold visited and seen values
    //seen means that the index is already in the result set 
    //and visited means the corresponding pre or post set was added to the
    //result
    bool ** seen = new bool*[2];
    bool ** visited = new bool *[2];

    seen[TR] = new bool[cardTR]();
    seen[PL] = new bool[cardPL]();

    visited[TR] = new bool[cardTR]();
    visited[PL] = new bool[cardPL]();


    // go through nodetypes 
    for (int nodetype = PL; nodetype <= TR; ++nodetype)
    {
        // go through nodes
        for (arrayindex_t index = 0; index < Net::Card[nodetype]; index ++)
        {
            // check if new root node was already seen
            if(!seen[nodetype][index])
            {
                //adding a rootnode
                uf_node root_node = {.index = -index , .parent = NULL, .type = nodetype};
                Net::UnionFind[union_find_ptr] = root_node;
                union_find_ptr++;

                // setting the lowest index a root element has
                UnionFind::rootNodeMax = -index;

                //set changes to true because we added a new node
                changes = true;

                //because its added its now seen
                seen[nodetype][index] = true;
            }
            //if it was already seen skip this node
            else
            {
                continue;
            }
            
            // as long as there are changes to the array go on searching
            while(changes){
                changes = false;

                //go trough result array
                for(arrayindex_t i = visited_ptr; i < union_find_ptr; i++){
                    // fetch the corresponding node from the result
                    uf_node_t * node = &Net::UnionFind[i];

                    //get its index
                    arrayindex_t node_index = node->index;

                    // check if it is a root node and make the index positive
                    // for working
                    if(node->index <= 0)
                    {
                        node_index = - node->index; 
                    }

                    //check if this node was already visited
                    if(visited[node->type][node_index])
                    {
                        continue;
                    }

                    // if node is place
                    if(node->type == PL)
                    {
                        //go through postset
                        for(arrayindex_t k = 0; k < Net::CardArcs[PL][POST][node_index];k++)
                        {
                            //get the index of this transition
                            arrayindex_t tr_index = Net::Arc[PL][POST][node_index][k];
                            //check if it already seen
                            if(!seen[TR][tr_index])
                            {
                                // add the new node
                                changes = true;
                                uf_node new_transition_node = {.index = tr_index, .parent = node, .type = TR}; 
                                Net::UnionFind[union_find_ptr] = new_transition_node;
                                union_find_ptr++;
                                seen[TR][tr_index] = true;
                            }
                        }
                    }
                    // if node is transition
                    else if(node->type == TR)
                    {
                        //go through preset
                        for(arrayindex_t k = 0 ; k < Net::CardArcs[TR][PRE][node_index];k++)
                        {
                            //get the index of this transition
                            arrayindex_t pl_index = Net::Arc[TR][PRE][node_index][k];
                            //check if it already seen
                            if(!seen[PL][pl_index])
                            {
                                // add the new node
                                changes=true;
                                uf_node new_place_node = {.index = pl_index, .parent = node, .type = PL}; 
                                Net::UnionFind[union_find_ptr] = new_place_node;
                                union_find_ptr++;
                                seen[PL][pl_index] = true;
                            }
                        }
                    }
                    // mark as visited
                    visited[node->type][node_index] = true;
                    visited_ptr++;
                }
            }
        }
    }
    //save the size of the cluster
    UnionFind::clusterSize = union_find_ptr;

    // free the allocated vars
    delete [] seen[TR];
    delete [] seen[PL];
    delete [] visited[TR];
    delete [] visited[PL];
    delete [] seen;
    delete [] visited;
}
/*!
 this function finds the root for a given index eg the corresponding cluster
 \param index the index of the node to search for
 \param type the type of the node to search for
 \return the index of the root node
 \post the nodes on the path from the starting point get the root node assigned as parent
*/
arrayindex_t UnionFind::find(arrayindex_t index, node_t type){
    std::stack<uf_node*> path_stack;
    for(arrayindex_t i=0; i < UnionFind::clusterSize;i++){
        if((Net::UnionFind[i].index == index || Net::UnionFind[i].index== -index) && Net::UnionFind[i].type==type ){

            uf_node * root= &Net::UnionFind[i];

            while(root->index > 0)
            {
                path_stack.push(root);
                root = root->parent;
            }
            while(!path_stack.empty()){
                path_stack.top()->parent = root;
                path_stack.pop();
            }
            return -root->index;
        }
    }
}


