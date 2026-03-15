#ifndef TYPES_H
#define TYPES_H

#include <cstdint>
#include <iostream>
#include <limits>
#include <stdlib.h>

typedef unsigned int ui;
typedef long long KeyID;
typedef short NodeID;

typedef uint32_t VertexID;
typedef ui LabelID;

const int MPI_MASTER = 0;
const int missingMId = -1;

const VertexID INVALID_VID(std::numeric_limits<VertexID>::max());
const NodeID INVALID_MID(std::numeric_limits<NodeID>::max());

struct wedge{
    VertexID u; 
    VertexID v; 
    VertexID w;

    // Constructor
    wedge(VertexID x, VertexID y, VertexID z) 
        : u(x), v(y), w(z) {}
};


struct Edge 
{
	static const unsigned short szAttr 	= 3;

	VertexID src;
	VertexID dst;
	bool add;

	Edge(): src(INVALID_VID), dst(INVALID_VID) {}
	Edge(VertexID iSrc, VertexID iDst): src(iSrc), dst(iDst) {}
    Edge(const Edge &iEdge): src(iEdge.src), dst(iEdge.dst) {}
	
    inline bool operator==(const Edge& iEdge) const 
	{
		return (src == iEdge.src) && (dst == iEdge.dst);
	}
    
	inline bool operator!=(const Edge& iEdge) const 
	{	
		return !operator==(iEdge);
	}

};

struct ElemCnt
{
	static const unsigned short szAttr = 2;
	VertexID	vid;
	double	cnt;
	inline void setValue(VertexID iVid, double iCnt)
	{
		vid = iVid;
		cnt = iCnt;
	}
};

#endif