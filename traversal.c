#include "traversal.h"
#include <time.h>
#include <math.h>

static Body *part;
static Node *tree; // thread number
static int FIRST_NODE;
double open_angle;
/*  
int accepted_cell_branes(int one, int ns, Node *tree, Body *part)
{
    int d, n, id;
    double dx[3], dr;
    if (one >= FIRST_NODE) {
        dr = 0.0;
        for (d=0; d<DIM; d++) {
            dx[d] = tree[ns].masscenter[d] - tree[one].masscenter[d];
            dr += dx[d]*dx[d];
        }
        dr = sqrt(dr) - tree[one].width ;
        if ( (tree[ns].width) < open_angle*dr )
            return 1;
        else
            return 0;
    }
    else if (one > -1)  {
        dr = 0.0;
        for (d=0; d<DIM; d++) {
            dx[d] = tree[ns].masscenter[d] - part[one].pos[d];
            dr += dx[d]*dx[d];
        }
        dr = sqrt(dr);
        if ( (tree[ns].width) < open_angle*dr )
            return 1;
        else
            return 0;
    }
    printf("error\n");
}

void naive_walk_cell(int add, int level, int body, Node *tree, Body *part)
{
    int id, n, d;
    double dx[3], dr3, dr2;
    if (add == body)
        return;

    if (add<FIRST_NODE) {
        dr2 = 0.0;
        for (d=0; d<DIM; d++) {
            dx[d] = part[add].pos[d] - part[body].pos[d];
            dr2 += dx[d]*dx[d];
        }
        dr3 = dr2 * sqrt(dr2);
        for (d=0; d<DIM; d++) {
            part[body].acc[d] += dx[d]/dr3;
        }
    }
    else
        for (n=0; n<8; n++) {
            id = tree[add].sub[n];
            if (id>-1) {
                if ( id >= FIRST_NODE ) {
                    if (accepted_cell_branes(body, id, tree, part)) {
                        dr2 = 0.0;
                        for (d=0; d<DIM; d++) {
                            dx[d] = tree[id].masscenter[d]-part[body].pos[d];
                            dr2 += dx[d]*dx[d];
                        }
                        dr3 = dr2 * sqrt(dr2);
                        for (d=0; d<DIM; d++) {
                            part[body].acc[d] += (tree[id].nPart) * dx[d]/dr3;
                        }
                    }
                    else
                        naive_walk_cell(id, level+1, body, tree, part);
                }
                else
                    naive_walk_cell(id, level+1, body, tree, part);
            }
        }
}
*/

/* task pool */
/*  
void inner_traversal(Domain *dp, GlobalParam *gp, int nThread) {
    int i,j,k, ic, jc, kc;
    int In, Jn, Kn;
    int n, level;
    int group, index;
    int first_node;
    int npart;

    Node *tree;
    Body* part;
    DomainTree *dtp = dp->domtree;

    npart = dp->NumPart;
    part = dp->Part;
    tree = dp->domtree->tree;
    FIRST_NODE = dp->NumPart;

    open_angle = 0.3;
    level = gp->NumBits;
    int cnt = 0;
    In = dp->cuboid->nSide[0];
    Jn = dp->cuboid->nSide[1];
    Kn = dp->cuboid->nSide[2];

    for (n=0; n<npart; n++) {
        if (TAG_OUTERCORE == part[n].tag) {
            index = part[n].group;
            first_node = dtp->root_cell[index];
            naive_walk_cell(first_node, level, n, tree, part);
            cnt ++;
        }
    }
    printf("cnt = %d\n", cnt);

}
*/

////////////////////////////////////////////////////////////////////////////////////////////////

/*
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define DIM 3
typedef double VECTOR[DIM];

typedef struct {
    VECTOR pos;
    VECTOR vel;
    VECTOR acc;
    double phi;
    long int ph_key;
    long int id;
} BODY;

typedef struct _node {
    int    nPart;
    int    sub[8];
    double width;
    VECTOR masscenter;
} NODE;


NODE *tree; // id of nodes
BODY *part;
static int NODELENGTH;
static int FIRST_NODE; // nPart
static int LAST_NODE;

void insert_particle_into_tree(VECTOR center,double size,int iNode,int iPart)
{
    int d, index, old, num;
    double qsize = 0.25*size;

    for (d=0, index=0; d<DIM; d++)
        if (part[iPart].pos[d] > center[d])
            index |= 1<<d ;

    num = tree[iNode].nPart ;
    tree[iNode].nPart += 1;
    tree[iNode].width = size;
    for (d=0; d<DIM; d++) {
        tree[iNode].masscenter[d] *= num;
        tree[iNode].masscenter[d] += part[iPart].pos[d];
        tree[iNode].masscenter[d] /= tree[iNode].nPart;
    }
    // this is empty
    if ( tree[iNode].sub[index] < 0) {
        tree[iNode].sub[index] = iPart;
        return;
    }
    for (d=0; d<DIM; d++)
        center[d] += (part[iPart].pos[d] > center[d]) ? qsize : -qsize ;
    // this is a particle
    if ( tree[iNode].sub[index] < FIRST_NODE ) {
        old = tree[iNode].sub[index]; // old particle;
        tree[iNode].sub[index] = ( ++LAST_NODE ) ; // new node
        insert_particle_into_tree(center, size/2, LAST_NODE, old);
    }
    // this is a node
    insert_particle_into_tree(center, size/2, tree[iNode].sub[index], iPart);
}

void construct_tree(int nPart, double size)
{
    int d, n;
    VECTOR center;
    FIRST_NODE = nPart; // nPart
    LAST_NODE  = nPart;
    NODELENGTH = nPart;

    tree = (NODE*)malloc( sizeof(NODE) * NODELENGTH );
    for (n=0; n<NODELENGTH; n++) {
        tree[n].nPart = 0;
        for (d=0; d<(1<<DIM); d++)
            tree[n].sub[d] = -1;
    }
    tree -= FIRST_NODE;

    for (n=0; n<nPart; n++) {
        for (d=0; d<DIM; d++)
            center[d] = size/2;
        insert_particle_into_tree(center, size, FIRST_NODE, n);
    }
}

int *opencell;
int *accept;
double open_angle = 0.3;
double box ;

// only for particle-node
int accepted_cell_gadget(int one, int ns)
{
    double alpha = 0.001;
    int d, n, id;
    double dx[3], dr2, ext2;

        ext2 = (tree[ns].width) * (tree[ns].width) ;
        dr2 = 0.0;
        for (d=0; d<DIM; d++) {
            dx[d] = tree[ns].masscenter[d] - part[one].pos[d];
            dr2 += dx[d]*dx[d];
        }

        if ( (tree[ns].nPart)*ext2/dr2/dr2 <= alpha*(part[one].phi)) {
            return 1;
        }
        else
            return 0;

}

int accepted_cell_branes(int one, int ns)
{
    int d, n, id;
    double dx[3], dr;
    if (one >= FIRST_NODE) {
        dr = 0.0;
        for (d=0; d<DIM; d++) {
            dx[d] = tree[ns].masscenter[d] - tree[one].masscenter[d];
            dr += dx[d]*dx[d];
        }
        dr = sqrt(dr) - tree[one].width ;
        if ( (tree[ns].width/dr) < open_angle )
            return 1;
        else
            return 0;
    }
    else if (one > -1)  {
        dr = 0.0;
        for (d=0; d<DIM; d++) {
            dx[d] = tree[ns].masscenter[d] - part[one].pos[d];
            dr += dx[d]*dx[d];
        }
        dr = sqrt(dr);
        if ( (tree[ns].width/dr) < open_angle )
            return 1;
        else
            return 0;
    }
    printf("error\n");
}

int accepted_cell(int one, int ns) {
   // return accepted_cell_gadget(one, ns);
      return accepted_cell_branes(one, ns);
}

void gravity_sumover_accepted(int body,  int id_acc)
{
    int d, n, id;
    double dx[3], dr3, dr2;
    for (d=0; d<DIM; d++)
        part[body].acc[d] = 0.0;

//   printf("\n");
    for (n=0; n<id_acc; n++) {
        id = accept[n];
        //     printf("%d ", id);
        if (id != body)
            if (id >= FIRST_NODE ) {
                dr2 = 0.0;
                for (d=0; d<DIM; d++) {
                    dx[d] =  tree[id].masscenter[d] - part[body].pos[d];
                    dr2 += dx[d]*dx[d];
                }
                dr3 = dr2 * sqrt(dr2);
                for (d=0; d<DIM; d++) {
                    part[body].acc[d] += (tree[id].nPart) * dx[d]/dr3;
                }
            }
            else if (id > -1 ) {
                dr2 = 0.0;
                for (d=0; d<DIM; d++) {
                    dx[d] =  part[id].pos[d] - part[body].pos[d];
                    dr2 += dx[d]*dx[d];
                }
                dr3 = dr2 * sqrt(dr2);
                for (d=0; d<DIM; d++) {
                    part[body].acc[d] += dx[d]/dr3;
                }
            }
    }
}

void walk_cell(int one, int level, int os, int oe, int apt)
{
    int id, n, ns, ns2, cell;
    int new_os, new_oe, new_mid_oe, new_apt, new_mid_apt;

    if (one < 0)
        printf("ont a node or body\n");

    if ( os == oe && one < FIRST_NODE) {
        gravity_sumover_accepted(one, apt);
    }

    if ( os < oe && one < FIRST_NODE) {
        new_os = new_oe = oe+1;
        new_apt = apt;
        for (ns=os; ns<oe; ns++) {
            cell = opencell[ns] ;
            if ( accepted_cell(one, cell) ) {
                accept[new_apt] = cell;
                new_apt++;
            }
            else {
                for (n=0; n<8; n++) {
                    if (tree[cell].sub[n] >= FIRST_NODE) {
                        opencell[new_oe] = tree[cell].sub[n];
                        new_oe ++ ;
                    }
                    else if (tree[cell].sub[n] > -1 ) {
                        accept[new_apt] = tree[cell].sub[n];
                        new_apt++;
                    }
                }
            }
        }
        walk_cell(one, level, new_os, new_oe, new_apt);
    }

    if ( os == oe && one >= FIRST_NODE) {
        for (n=0; n<8; n++)
            if ( (id = tree[one].sub[n]) > -1 )
            {
                new_os = new_oe = oe+1;
                new_apt = apt;
                for (ns=n+1; ns<n+8; ns++) {
                    if (tree[one].sub[ns%8] >= FIRST_NODE) {
                        opencell[new_oe] = tree[one].sub[ns%8];
                        new_oe ++ ;
                    }
                    else if (tree[one].sub[ns%8] > -1) {
                        accept[new_apt] = tree[one].sub[ns%8];
                        new_apt++;
                    }
                }
                walk_cell(id, level+1, new_os, new_oe, new_apt);
            }
    }

    if ( os < oe && one >= FIRST_NODE) {
        for (n=0; n<8; n++)
            if ( (id = tree[one].sub[n]) > -1 ) {
                new_os = new_oe = oe+1;
                new_apt = apt;
                for (ns=n+1; ns<n+8; ns++) {
                    if (tree[one].sub[ns%8] >= FIRST_NODE) {
                        opencell[new_oe] = tree[one].sub[ns%8];
                        new_oe ++ ;
                    }
                    else if (tree[one].sub[ns%8] > -1) {
                        accept[new_apt] = tree[one].sub[ns%8];
                        new_apt++;
                    }
                }
                for (ns=os; ns<oe; ns++) {
                    cell = opencell[ns] ;
                    if ( accepted_cell(id, cell) ) {
                        accept[new_apt] = cell;
                        new_apt++;
                    }
                    else {
                        for (ns2=0; ns2<8; ns2++) {
                            if (tree[cell].sub[ns2] >= FIRST_NODE) {
                                opencell[new_oe] = tree[cell].sub[ns2];
                                new_oe ++ ;
                            }
                            else if (tree[cell].sub[ns2] > -1 ) {
                                accept[new_apt] = tree[cell].sub[ns2];
                                new_apt++;
                            }
                        }
                    }
                }
                walk_cell(id, level+1, new_os, new_oe, new_apt);
            }
    }
}

void walk_tree_for_gravity(int nPart)
{
    clock_t begin, end;
    double time_spent;
    begin = clock();
    opencell = (int*)malloc(sizeof(int)*5/2*nPart);
    accept   = (int*)malloc(sizeof(int)*5/2*nPart);
    opencell[0] = FIRST_NODE;
    walk_cell(FIRST_NODE, 0, 0, 0, 0);
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf(" recursive walk [%d] time = %lf[sec]\n", nPart, time_spent);
}

void naive_walk_cell(int add, int level, int body)
{
    int id, n, d;
    double dx[3], dr3, dr2;
    if (add == body)
        return;

    if (add<FIRST_NODE) {
        dr2 = 0.0;
        for (d=0; d<DIM; d++) {
            dx[d] = part[add].pos[d] - part[body].pos[d];
            dr2 += dx[d]*dx[d];
        }
        dr3 = dr2 * sqrt(dr2);
        for (d=0; d<DIM; d++) {
            part[body].acc[d] += dx[d]/dr3;
        }
    }
    else
        for (n=0; n<8; n++) {
            id = tree[add].sub[n];
            if (id>-1) {
                if ( id >= FIRST_NODE ) {
                    if (accepted_cell(body, id)) {
                        dr2 = 0.0;
                        for (d=0; d<DIM; d++) {
                            dx[d] = tree[id].masscenter[d]-part[body].pos[d];
                            dr2 += dx[d]*dx[d];
                        }
                        dr3 = dr2 * sqrt(dr2);
                        for (d=0; d<DIM; d++) {
                            part[body].acc[d] += (tree[id].nPart) * dx[d]/dr3;
                        }
                    }
                    else
                        naive_walk_cell(id, level+1, body);
                }
                else
                    naive_walk_cell(id, level+1, body);
            }
        }
}

void naive_walk_tree_for_gravity(int nPart)
{
    int d, n;
    clock_t begin, end;
    double time_spent;
    begin = clock();
    for (n=0; n<nPart; n++) {
        for (d=0; d<DIM; d++)
            part[n].acc[d] = 0.0;

        naive_walk_cell(FIRST_NODE, 0, n);
    }
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf(" naive_walk [%d] time = %lf[sec]\n", nPart, time_spent);
}

void walk_tree(int add, int level)
{
    int id, n;
    printf("[%d]%d included %d width = %f\n", level, add, tree[add].nPart, tree[add].width);
    for (n=0; n<8; n++) {
        id = tree[add].sub[n];

        if ( id > FIRST_NODE )  // nodes
        {
            walk_tree(id, level+1) ;
        }
        else if ( id >= 0) // particles
        {
            printf(" [%d.%d]-> %d : %f %f %f \n", add, n, id, part[id].pos[0], part[id].pos[1], part[id].pos[2]);
        }
    }
}

float ran3(long *idum);
void direct_addition(BODY *bptr, int nPart);

int main(void)
{
    int d, n, nNode;
    int nPart;
    int level;
    long int seed;
    double center[3], box;
    clock_t begin, end;
    double time_spent;
    FILE *fd;

    nPart = 20000000;
    box = 1.0;
    seed = 4658714;
    part = (BODY*)malloc( sizeof (BODY)*nPart );
    for (n=0; n<nPart; n++) {
        for (d=0; d<DIM; d++) {
            part[n].pos[d] = box*ran3(&seed);
        }
    }

    for (n=0; n<nPart; n++) {
        part[n].phi = 0.0;
        for (d=0; d<DIM; d++) {
            part[n].phi += part[n].acc[d]* part[n].acc[d];
        }
        part[n].phi = sqrt(part[n].phi);
    }

    begin = clock();
    construct_tree(nPart, box) ;
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("construct tree [%d] time = %lf[sec]\n", nPart, time_spent);

    walk_tree_for_gravity(nPart);
    fd = fopen("nbody_recursive_treewalk.txt","w");
    fprintf(fd, "%d %lf\n", nPart, box);
    for (n=0; n<nPart; n++) {
        fprintf(fd, "%lf %lf %lf %e %e %e\n", (part[n].pos[0]), (part[n].pos[1]), (part[n].pos[2]), (part[n].acc[0]), (part[n].acc[1]), (part[n].acc[2]));
    }
    fclose(fd);

    naive_walk_tree_for_gravity(nPart) ;

    fd = fopen("nbody_iterative_treewalk.txt","w");
    fprintf(fd, "%d %lf\n", nPart, box);
    for (n=0; n<nPart; n++) {
        fprintf(fd, "%lf %lf %lf %e %e %e\n",(part[n].pos[0]), (part[n].pos[1]), (part[n].pos[2]), (part[n].acc[0]), (part[n].acc[1]), (part[n].acc[2]));
    }
    fclose(fd);

//   free(part);
    return 0;
}
*/
int threaded(pthread_t* thread, Block* pth, int ts, int te, PPQ *pq, TWQ *tq, int* lower,int* upper, Domain*dp,GlobalParam*gp,void* process(void*))
{
	int i,teamid;
    printf("before threaded\n");
//	sleep(2);	
	for (i=ts; i<te+1; i++)
	{
		pth[i].blockid = i/NTEAM;
		pth[i].teamid=i%NTEAM;
		teamid=pth[i].teamid;
		pth[i].bsize = MASTER+NSLAVE;
		pth[i].tid = i;
		pth[i].tall = NTEAM*(MASTER+NSLAVE);
		pth[i].PQ_ppnode = &pq[teamid];
		pth[i].PQ_treewalk = &tq[teamid];
		pth[i].first=lower[teamid];
		pth[i].last=upper[teamid];
		
		pth[i].dp=dp;
		pth[i].gp=gp;
		pth[i].thread=thread;
		pth[i].pthArr=pth;
		pth[i].lower=lower;
		pth[i].upper=upper;
		pth[i].P_PQ_ppnode=pq;
		pth[i].P_PQ_treewalk=tq;
		pthread_create(&thread[i], NULL, process, (void*)&pth[i]);
	}
}

void* compPP(void* param){
	Array3 pa, pb, pc;
	int i,j,k,n,m,l;
	Block* pth=(Block*)param;
	int blockid=pth->blockid;
	int teamid=pth->teamid;
	int bsize=pth->bsize;
	int tid=pth->tid;
	int tall=pth->tall;
	PPQ* PQ_ppnode=pth->PQ_ppnode;
	TWQ* PQ_treewalk=pth->PQ_treewalk;
	Domain* dp=pth->dp;
	GlobalParam* gp=pth->gp;
	pthread_t* thread=pth->thread;
	Block* pthArr=pth->pthArr;
	
//	printf("\nQueue process finishing, total %d pp pairs.\n", PQ_ppnode->length);
	pa.x = (PRECTYPE*)malloc(sizeof(PRECTYPE)*MAX_PACKAGE_SIZE*(1<<(MAX_MORTON_LEVEL*3)));
	pa.y = (PRECTYPE*)malloc(sizeof(PRECTYPE)*MAX_PACKAGE_SIZE*(1<<(MAX_MORTON_LEVEL*3)));
	pa.z = (PRECTYPE*)malloc(sizeof(PRECTYPE)*MAX_PACKAGE_SIZE*(1<<(MAX_MORTON_LEVEL*3)));
	pb.x = (PRECTYPE*)malloc(sizeof(PRECTYPE)*MAX_PACKAGE_SIZE*(1<<(MAX_MORTON_LEVEL*3)));
	pb.y = (PRECTYPE*)malloc(sizeof(PRECTYPE)*MAX_PACKAGE_SIZE*(1<<(MAX_MORTON_LEVEL*3)));
	pb.z = (PRECTYPE*)malloc(sizeof(PRECTYPE)*MAX_PACKAGE_SIZE*(1<<(MAX_MORTON_LEVEL*3)));
	pc.x = (PRECTYPE*)malloc(sizeof(PRECTYPE)*MAX_PACKAGE_SIZE*(1<<(MAX_MORTON_LEVEL*3)));
	pc.y = (PRECTYPE*)malloc(sizeof(PRECTYPE)*MAX_PACKAGE_SIZE*(1<<(MAX_MORTON_LEVEL*3)));
	pc.z = (PRECTYPE*)malloc(sizeof(PRECTYPE)*MAX_PACKAGE_SIZE*(1<<(MAX_MORTON_LEVEL*3)));

    printf("before ppkernel\n");
	sleep(5);	
	while(PQ_ppnode->length>0||PQ_ppnode->tag==0)
	{
		if(PQ_ppnode->length>0)
		ProcessQP_PPnode(PQ_ppnode, ppkernel, pa, pb, pc);
	}
    printf("after ppkernel\n");	
	printf("\nPP processing finished.\n");

	free(pa.x);
	free(pa.y);
	free(pa.z);
	free(pb.x);
	free(pb.y);
	free(pb.z);
	free(pc.x);
	free(pc.y);
	free(pc.z);

//	destroy_queue_pp(&PQ_ppnode);
//	destroy_queue_tw(&PQ_treewalk);
		
}

void* teamMaster(void* param){
	
	int i,j,k,n,m,l,i1,i2,i3,cella;
	Block* pth=(Block*)param;
	int blockid=pth->blockid;
	int teamid=pth->teamid;
	int bsize=pth->bsize;
	int tid=pth->tid;
	int tall=pth->tall;
	PPQ* PQ_ppnode=pth->PQ_ppnode;
	TWQ* PQ_treewalk=pth->PQ_treewalk;
	int first=pth->first;
	int last=pth->last;
	Domain* dp=pth->dp;
	GlobalParam* gp=pth->gp;
	pthread_t* thread=pth->thread;
	Block* pthArr=pth->pthArr;
	int* lower=pth->lower;
	int* upper=pth->upper;
	PPQ* P_PQ_ppnode=pth->P_PQ_ppnode;
	TWQ* P_PQ_treewalk=pth->P_PQ_treewalk;
	
    int In = dp->cuboid->nSide[0];
    int Jn = dp->cuboid->nSide[1];
    int Kn = dp->cuboid->nSide[2];
    int cnt = 0;
    open_angle = 0.3;

    DomainTree *dtp = dp->domtree;
    int npart = dp->NumPart;
    part = dp->Part;
    tree = dtp->tree;
    printf("first last %d,%d\n",first,last);
	for(i=first;i<last;i++){
		i1=i/(Jn*Kn);
		if(i1==0||i1==In-1)
			continue;	
		i2=(i-i1*Jn*Kn)/Kn;
		if(i2==0||i2==Jn-1)
			continue;	
		i3=i-i1*Jn*Kn-i2*Kn;
		if(i3==0||i3==Kn-1)
			continue;
		cella=i;	
				for (n=i1-1; n<i1+2; n++)
				{
					for (m=i2-1; m<i2+2; m++)
					{
						for (l=i3-1; l<i3+2; l++)
						{
							int cellb = n*(Jn*Kn)+m*Kn+l;
							if (dtp->numparticles[cella]>0 && dtp->numparticles[cellb]>0)
							{
								if(dtp->root_cell[cella]==0)
								printf("enqueue: i:%d, j:%d, k:%d, n:%d, m:%d, l:%d, ta:%d, tb:%d, na=%d, nb=%d\n", i, j, k, n, m, l, dtp->root_cell[cella], dtp->root_cell[cellb], tree[dtp->root_cell[cella]].nPart, tree[dtp->root_cell[cellb]].nPart);
								EnqueueP_Cell(PQ_treewalk, dtp->root_cell[cella], dtp->root_cell[cellb], open_angle);
//								printf("E");
								cnt++;
							}
						}
					}
				}
	}
	printf("\ncnt = %d\n", cnt);
	
	PQ_ppnode->tag=0;
	threaded(thread, pthArr, NTEAM+NSLAVE*teamid, NTEAM+NSLAVE*(teamid+1)-1, P_PQ_ppnode, P_PQ_treewalk, lower, upper, dp,gp,compPP);

	while(PQ_treewalk->length>0)
	{
		ProcessQP_Cell(PQ_treewalk, PQ_ppnode, dtt_process_cell);
	}
	PQ_ppnode->tag=1;
	printf("\nQueue process finishing, total %d pp pairs.\n", PQ_ppnode->length);
	
}


// Cao! Use this critertian for cell/cell acceptance judging. 
int accepted_cell_to_cell(int TA, int TB, double theta/* Here theta is the open_angle  */)
{
	double delta, dr;
	int d;

//# Strict substraction of R for correct acceptance:
//	double Rmax = SQROOT3 * (pTA->bmax + pTB->bmax); 
//#	Alternative subtraction of R:
	double Rmax = tree[TB].width;
	double Bmax = tree[TA].width;
//	double Rmax = tree[TA].width + tree[TB].width;
//	double Bmax = 0.0;

	// Tree node include test.
	if (tree[TA].level < tree[TB].level)
	{
		if (tree[TA].firstpart<=tree[TB].firstpart && tree[TA].firstpart+tree[TA].nPart>=tree[TB].firstpart)
		{
			return 0;
		}
	}
	else if (tree[TA].level > tree[TB].level)
	{
		if (tree[TB].firstpart<=tree[TA].firstpart && tree[TB].firstpart+tree[TB].nPart>=tree[TA].firstpart)
		{
			return 0;
		}
	}

	dr = 0.0;

//	printf("accept.\n");
//	sleep(10);
	for (d=0; d<DIM; d++)
	{
		delta = tree[TB].masscenter[d] - tree[TA].masscenter[d];
		dr += delta*delta;
	}

	dr = SQRT(dr) - Bmax;
	if ( Rmax < theta * dr )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

// Cao! Walk the tree using Dual Tree Traversel algorithm.
int dtt_process_cell(int TA, int TB, double theta, PPQ *PQ_ppnode, TWQ *PQ_treewalk)
{
//	assert ((tree[TA].level >=0) && (tree[TA].level <= MAX_MORTON_LEVEL));
//	assert ((tree[TB].level >=0) && (tree[TB].level <= MAX_MORTON_LEVEL));

//	printf("process. TA=%d, TB=%d, theta=%.2f.\n", TA, TB, theta);

	if(TA == TB)
	{
		if (tree[TA].childnum == 0)
		{
			/* Cao! Here we send the tree node, and the Queue      */
			/*      processing function should pack the particles  */
			/*      and do some further optimizatoins.             */
			/*      PQ_ppnode: node pp packs, !0 means accepted    */
			/*      PQ_particle: save the packed pps.              */
			EnqueueP_PPnode(PQ_ppnode, TA, TB, 0);
		}
		else
		{
			int i;
			for (i=0; i<tree[TA].childnum; i++)
			{
				EnqueueP_Cell(PQ_treewalk, tree[TA].firstchild+i, TB, theta);
			}
		}
	}
	else
	{
		if (accepted_cell_to_cell(TA, TB, theta))
		{
			/* Cao! See comments above. */
			EnqueueP_PPnode(PQ_ppnode, TA, TB, 1);
		}
		else
		{
			if ( tree[TA].childnum==0 && tree[TB].childnum==0 )
			{
				/* Cao! See comments above. */
				EnqueueP_PPnode(PQ_ppnode, TA, TB, 0);
			}
			else if ( tree[TB].childnum==0 || (tree[TA].childnum>0 && tree[TA].width>tree[TB].width) )
//			else if ( tree[TB].childnum==0 || tree[TA].childnum>0 ) // Open A until reach the leaf.
			{
				int i;
				for (i=0; i<tree[TA].childnum; i++)
				{
					EnqueueP_Cell(PQ_treewalk, tree[TA].firstchild+i, TB, theta);
				}
			}
			else
			{
				int i;
				for (i=0; i<tree[TB].childnum; i++)
				{
					EnqueueP_Cell(PQ_treewalk, TA, tree[TB].firstchild+i, theta);
				}
			}
		}
	}
}

void dtt_traversal(Domain *dp, GlobalParam *gp)
{
    int i, j, k, m, n, l;
    int In, Jn, Kn;
    int npart;

    npart = dp->NumPart;
	int cnt = 0;
    In = (dp->cuboid)->nSide[0];
    Jn = (dp->cuboid)->nSide[1];
    Kn = (dp->cuboid)->nSide[2];
    DomainTree *dtp = dp->domtree;
    part = dp->Part;
    tree = dp->domtree->tree;

	TWQ *P_PQ_treewalk;
	PPQ *P_PQ_ppnode;

	Block* pth;
	pthread_t* thread;
	
	int tnum=NTEAM*(MASTER+NSLAVE);
	pth = (Block*)malloc(sizeof(Block)*tnum);
	thread = (pthread_t*)malloc(sizeof(pthread_t)*tnum);
	P_PQ_ppnode=(PPQ*)malloc(sizeof(PPQ)*NTEAM);
	P_PQ_treewalk=(TWQ*)malloc(sizeof(TWQ)*NTEAM);

	initGlobal(part, tree);
	printf("Init queue ppnode.\n");
	init_queue_pp(P_PQ_ppnode);
	printf("Init queue treewalk.\n");
	init_queue_tw(P_PQ_treewalk);

	printf("real domain In-2:%d, Jn-2:%d, Kn-2:%d\n", In-2, Jn-2, Kn-2);
	int *accum,*lower,*upper,*numpart;
	int accum_thread;
	int Ngrid=In*Jn*Kn;
	int* count=(dp->cuboid)->count;
	accum_thread=npart/NTEAM;
	accum=(int*)malloc(sizeof(int)*Ngrid);
	accum[0]=count[0];
    for (n=1; n<Ngrid; n++) {
        accum[n] = accum[n-1] + count[n];
    }
    lower = (int*)malloc(sizeof(int)*NTEAM);
    upper = (int*)malloc(sizeof(int)*NTEAM);
    numpart = (int*)malloc(sizeof(int)*NTEAM);

    lower[0] = 0;
    for (m=1, n=0; n<Ngrid; n++) {
        if ( accum[n] > m*accum_thread ) {
            lower[m] = upper[m-1] = n;
            m++;
        }
    }
    upper[NTEAM-1]=Ngrid;

    numpart[0] = accum[upper[0]-1];
    for (m=1; m<NTEAM; m++) {
        numpart[m] = accum[upper[m]-1] - accum[lower[m]-1];
    }
    printf("before teamMaster\n");	
	threaded(thread, pth, 0, NTEAM-1, P_PQ_ppnode, P_PQ_treewalk, lower, upper, dp,gp,teamMaster);
    printf("after teamMaster\n");	
	for (i=0; i<tnum; i++)
	{
		pthread_join(thread[i], NULL);
	}
}
