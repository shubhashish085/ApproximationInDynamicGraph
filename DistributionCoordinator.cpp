#include "DistributionCoordinator.hpp"

MPI_Datatype 		DistributionCoordinator::MPI_TYPE_EDGE;
MPI_Datatype 		DistributionCoordinator::MPI_TYPE_ELEMCNT;
const Edge 			DistributionCoordinator::END_STREAM(INVALID_VID, INVALID_VID);

unsigned short		DistributionCoordinator::lenBuf;

DistributionCoordinator::DistributionCoordinator(int &argc, char** &argv)//, bit(0), lenCQ(1), cqit(0)
{
	// Establish connection
	MPI_Init(&argc, &argv);

	// Get connection information
	MPI_Comm_size(MPI_COMM_WORLD, &szProc);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// Initialize and Register struct EDGE, ELEMCNT information
	int          lenAttr[Edge::szAttr] = {1, 1};
	MPI_Datatype arrType[Edge::szAttr] = {MPI_UNSIGNED, MPI_UNSIGNED, MPI_C_BOOL};

	MPI_Aint     offsets[Edge::szAttr];
	offsets[0] = offsetof(Edge, src);
	offsets[1] = offsetof(Edge, dst);
	offsets[2] = offsetof(Edge, add);
	MPI_Type_create_struct(Edge::szAttr, lenAttr, offsets, arrType, &MPI_TYPE_EDGE);
	MPI_Type_commit(&MPI_TYPE_EDGE);

	arrType[0] = MPI_UNSIGNED;
	arrType[1] = MPI_DOUBLE;
	offsets[0] = offsetof(ElemCnt, vid);
	offsets[1] = offsetof(ElemCnt, cnt);
	MPI_Type_create_struct(ElemCnt::szAttr, lenAttr, offsets, arrType, &MPI_TYPE_ELEMCNT);
	MPI_Type_commit(&MPI_TYPE_ELEMCNT);
}

// Initialize requests and buffers
void DistributionCoordinator::init(int lenBuf, int workerNum)
{
	commCostDistribute = 0;
    commCostGather = 0;
    ioCPUTime = 0;

    eBuf.clear();
    DistributionCoordinator::lenBuf = lenBuf;
    DistributionCoordinator::workerNum = workerNum;

    if (rank == MPI_MASTER)
    {
        eBuf.resize(workerNum);
        for (int i = 0; i < workerNum; i++)
        {
            eBuf[i].init(i);
        }
    }
    else
    {
        eBuf.resize(1);
        eBuf[0].init(getWorkerId());
    }
}


bool DistributionCoordinator::isMaster()
{
    return rank == MPI_MASTER;
}

NodeID DistributionCoordinator::getWorkerId()
{
    return (NodeID)(rank - 1);
}

long DistributionCoordinator::getCommCostDistribute()
{
    return commCostDistribute;
}

long DistributionCoordinator::getCommCostGather()
{
    return commCostGather;

}

void DistributionCoordinator::cleanup()
{
	MPI_Finalize();
}

bool DistributionCoordinator::bCastEdge(Edge &iEdge)
{
	Edge tmpEdge(iEdge);
	for (int mit = 0; mit < workerNum; mit++)
	{
		eBuf[mit].putNext(tmpEdge);
	}
	commCostDistribute += workerNum;

	return true;
}

bool DistributionCoordinator::IrecvEdge(Edge *buf, MPI_Request &iReq)
{
	return (MPI_SUCCESS == MPI_Irecv(buf, lenBuf, MPI_TYPE_EDGE, MPI_MASTER, TAG_STREAM, MPI_COMM_WORLD, &iReq));
	//waitIOCompletion(iReq);
}

bool DistributionCoordinator::IsendEdge(Edge *buf, int mid, MPI_Request &iReq){
	MPI_Isend(buf, lenBuf, MPI_TYPE_EDGE, mid + 1, TAG_STREAM, MPI_COMM_WORLD, &iReq);
	return true;
}

bool DistributionCoordinator::recvEdge(Edge &oEdge)
{
	eBuf[0].getNext(oEdge);
	if (oEdge == END_STREAM)
	{
		eBuf[0].cleanup();
	}
	return (oEdge != END_STREAM);
}

bool DistributionCoordinator::sendEndSignal()
{
	Edge signal(END_STREAM);
	for (int mit = 0; mit < workerNum; mit++)
	{
		eBuf[mit].putNext(signal);
		eBuf[mit].flushSend();
	}
	return true;
}


bool DistributionCoordinator::sendCnt(double gCnt, std::unordered_map<VertexID, float> &lCnt)
{
    clock_t begin = clock();
	MPI_Reduce(&gCnt, nullptr, 1, MPI_DOUBLE, MPI_SUM, MPI_MASTER, MPI_COMM_WORLD);
    VertexID maxVId;
    MPI_Bcast(&maxVId, 1, MPI_UNSIGNED, MPI_MASTER, MPI_COMM_WORLD);
    ioCPUTime += double(clock() - begin);


    float* lCntArr = new float[maxVId+1];
    std::fill_n(lCntArr, maxVId+1, 0.0);
    std::unordered_map<VertexID, float>::const_iterator it;
    for (it = lCnt.begin(); it != lCnt.end(); it++) {
        lCntArr[it->first] = it->second;
    }

    begin = clock();
    MPI_Reduce(lCntArr, nullptr, maxVId+1, MPI_FLOAT, MPI_SUM, MPI_MASTER, MPI_COMM_WORLD);
    ioCPUTime += double(clock() - begin);

    delete lCntArr;
    return true;

}

bool DistributionCoordinator::recvCnt(VertexID maxVId, double &gCnt, std::vector<float> &lCnt)
{
	//TODO. unsinged int -> unsigned long long?

	double empty = 0;
    clock_t begin = clock();
	MPI_Reduce(&empty, &gCnt, 1, MPI_DOUBLE, MPI_SUM, MPI_MASTER, MPI_COMM_WORLD);
    MPI_Bcast(&maxVId, 1, MPI_UNSIGNED, MPI_MASTER, MPI_COMM_WORLD);
    ioCPUTime += double(clock() - begin);

    float* lCntArr = new float[maxVId+1];
    std::fill_n(lCntArr, maxVId+1, 0.0);

    begin = clock();
    MPI_Reduce(MPI_IN_PLACE, lCntArr, maxVId+1, MPI_FLOAT, MPI_SUM, MPI_MASTER, MPI_COMM_WORLD);
    ioCPUTime += double(clock() - begin);

    commCostGather = (maxVId + 1) * (getSzProc()-1);

    lCnt.insert(lCnt.end(), &lCntArr[0], &lCntArr[maxVId]);
    delete lCntArr;

	return true;
}

double DistributionCoordinator::getIOCPUTime()
{
	if (rank == MPI_MASTER)
	{
		double totalIOCPUTime = ioCPUTime;
		for (int i = 0; i < workerNum; i++)
		{
			totalIOCPUTime += eBuf[i].ioCPUTime;
		}
		return totalIOCPUTime;
	}
	else
	{
		return eBuf[0].ioCPUTime;
	}
}

bool DistributionCoordinator::sendTime(double compTime)
{
    MPI_Reduce(&compTime, nullptr, 1, MPI_DOUBLE, MPI_MAX, MPI_MASTER, MPI_COMM_WORLD);
    MPI_Reduce(&compTime, nullptr, 1, MPI_DOUBLE, MPI_SUM, MPI_MASTER, MPI_COMM_WORLD);
    return true;
}

bool DistributionCoordinator::recvTime(double &compTimeMax, double &compTimeSum)
{
	double empty = 0;
	MPI_Reduce(&empty, &compTimeMax, 1, MPI_DOUBLE, MPI_MAX, MPI_MASTER, MPI_COMM_WORLD);
    MPI_Reduce(&empty, &compTimeSum, 1, MPI_DOUBLE, MPI_SUM, MPI_MASTER, MPI_COMM_WORLD);
	return true;
}
