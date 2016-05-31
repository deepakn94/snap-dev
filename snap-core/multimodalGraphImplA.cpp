/////////////////////////////////////////////////
// Multi-modal Graph, Impl A

TPair<TInt,TInt> TMultimodalGraphImplA::AddNode(const int ModeId) {
  int LocalNId = MxNId;
  MxNId++;
  Network.AddNode(LocalNId, ModeId);
  return TPair<TInt,TInt>(ModeId,LocalNId);
}

void TMultimodalGraphImplA::AddNode(const TPair<TInt,TInt>& NodeId) {
  Network.AddNode(NodeId.GetVal2(), NodeId.GetVal1());
}

void TMultimodalGraphImplA::GetNodeIdsInMode(const int ModeId, TVec< TPair<TInt,TInt> >& NodeIds) const {
  for (TIntNNet::TNodeI NI=BegNI(); NI<EndNI(); NI++) {
    if (NI.GetDat() == ModeId) {
      NodeIds.Add(TPair<TInt,TInt>(ModeId,NI.GetId()));
    }
  }
}

int TMultimodalGraphImplA::AddEdge(const TPair<TInt,TInt>& SrcNId, const TPair<TInt,TInt>& DstNId) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId.GetVal2(), DstNId.GetVal2()).CStr());
  if (IsEdge(SrcNId, DstNId)) {
    return -2; // Edge already exists
  }
  NEdges++;
  return Network.AddEdge(SrcNId.GetVal2(), DstNId.GetVal2());
}

void TMultimodalGraphImplA::AddEdgeBatch(const TPair<TInt,TInt>& SrcNId, const TVec<TPair<TInt,TInt> >& DstNIds) {
  IAssertR(IsNode(SrcNId), TStr::Fmt("%d not a node.", SrcNId.GetVal2()).CStr());
  Network.ReserveOutNIdV(SrcNId.GetVal2(), DstNIds.Len());
  for (TVec<TPair<TInt,TInt> >::TIter DstNId = DstNIds.BegI(); DstNId < DstNIds.EndI(); DstNId++) {
    AddEdge(SrcNId, *DstNId);
  }
}

void TMultimodalGraphImplA::DelEdge(const TPair<TInt,TInt>& SrcNId, const TPair<TInt,TInt>& DstNId) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId.GetVal2(), DstNId.GetVal2()).CStr());
  if (!IsEdge(SrcNId, DstNId)) {
    return; // Edge doesn't exist
  }
  NEdges--;
  Network.DelEdge(SrcNId.GetVal2(), DstNId.GetVal2());
}

TIntNNet TMultimodalGraphImplA::GetSubGraph(const TIntV ModeIds) const {
  TIntNNet SubGraph = TIntNNet();

  for (TIntNNet::TNodeI NI=BegNI(); NI<EndNI(); NI++) {
    if (ModeIds.IsIn(NI.GetDat())) {
      SubGraph.AddNode(NI.GetId(), NI.GetDat());
    }
  }

  for (TIntNNet::TNodeI NI=BegNI(); NI<EndNI(); NI++) {
    if (!ModeIds.IsIn(NI.GetDat())) {
      continue;
    }
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      int NeighboringNId = NI.GetOutNId(e);
      int NeighboringModeId = NI.GetOutNDat(e);
      if (!ModeIds.IsIn(NeighboringModeId)) {
        continue;
      }
      SubGraph.AddEdge(NI.GetId(), NeighboringNId);
    }
  }
  printf("Number of nodes in SubGraph: %d...\n", SubGraph.GetNodes());
  printf("Number of edges in SubGraph: %d...\n", SubGraph.GetEdges());

  return SubGraph;
}

int TMultimodalGraphImplA::GetSubGraphMocked(const TIntV ModeIds) const {
  int NumVerticesAndEdges = 0;

  for (TIntNNet::TNodeI NI=BegNI(); NI<EndNI(); NI++) {
    if (ModeIds.IsIn(NI.GetDat())) {
      NumVerticesAndEdges++;
    }
  }

  for (TIntNNet::TNodeI NI=BegNI(); NI<EndNI(); NI++) {
    if (!ModeIds.IsIn(NI.GetDat())) {
      continue;
    }
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      int NeighboringNId = NI.GetOutNId(e);
      int NeighboringModeId = NI.GetOutNDat(e);
      if (!ModeIds.IsIn(NeighboringModeId)) {
        continue;
      }
      NumVerticesAndEdges += NeighboringNId;
    }
  }

  return NumVerticesAndEdges;
}

int TMultimodalGraphImplA::BFSTraversalOneHop(const TVec< TPair<TInt,TInt> >& StartingVertices) const {
  int NumVerticesAndEdges = 0;
  for (int i = 0; i < StartingVertices.Len(); i++) {
    TIntNNet::TNodeI NI = GetNI(StartingVertices.GetVal(i));
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      NumVerticesAndEdges += NI.GetOutNId(e);
    }
  }
  return NumVerticesAndEdges;
}

void TMultimodalGraphImplA::RandomWalk(TVec<TInt> NodeIds, int WalkLength) {
  int CurrentNodeId = Network.GetRndNId();
  int NodeIdIdx = 0;
  NodeIds.Add(NodeIdIdx++, CurrentNodeId);
  while (NodeIds.Len() < WalkLength) {
    TIntNNet::TNodeI NI = Network.GetNI(CurrentNodeId);
    int EdgeId = TInt::Rnd.GetUniDevInt(NI.GetOutDeg());
    CurrentNodeId = NI.GetOutNId(EdgeId);
    NodeIds.SetVal(NodeIdIdx++, CurrentNodeId);
  }
}

PMultimodalGraphImplA TMultimodalGraphImplA::GetSmallGraph() {
  PMultimodalGraphImplA G = TMultimodalGraphImplA::New();
  TVec<TPair< TInt,TInt> > Nodes = TVec< TPair<TInt,TInt> >();
  for (int i = 0; i < 5; i++) { Nodes.Add(G->AddNode(0)); }
  for (int i = 0; i < 5; i++) { Nodes.Add(G->AddNode(1)); }
  G->AddEdge(Nodes.GetVal(0),Nodes.GetVal(1)); G->AddEdge(Nodes.GetVal(1),Nodes.GetVal(2));
  G->AddEdge(Nodes.GetVal(0),Nodes.GetVal(2)); G->AddEdge(Nodes.GetVal(1),Nodes.GetVal(3));
  G->AddEdge(Nodes.GetVal(3),Nodes.GetVal(4)); G->AddEdge(Nodes.GetVal(2),Nodes.GetVal(3));
  G->AddEdge(Nodes.GetVal(2),Nodes.GetVal(6)); G->AddEdge(Nodes.GetVal(4), Nodes.GetVal(8));
  G->AddEdge(Nodes.GetVal(5),Nodes.GetVal(6)); G->AddEdge(Nodes.GetVal(5),Nodes.GetVal(7));
  G->AddEdge(Nodes.GetVal(6),Nodes.GetVal(7)); G->AddEdge(Nodes.GetVal(6),Nodes.GetVal(9));
  G->AddEdge(Nodes.GetVal(6),Nodes.GetVal(8)); G->AddEdge(Nodes.GetVal(8),Nodes.GetVal(9));
  G->AddEdge(Nodes.GetVal(9),Nodes.GetVal(2)); G->AddEdge(Nodes.GetVal(5),Nodes.GetVal(0));
  G->DelNode(Nodes.GetVal(0));
  return G;
}

