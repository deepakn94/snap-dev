/////////////////////////////////////////////////
// Multi-modal Graph, Impl C

int TMultimodalGraphImplC::GetNodes() const {
  int NumNodes = 0;
  for (TNodeH::TIter Node=NodeH.BegI(); Node<NodeH.EndI(); Node++) {
    NumNodes += Node.GetDat().Len();
  }
  return NumNodes;
}

TPair<TInt,TInt> TMultimodalGraphImplC::AddNode(int ModeId) {
  int LocalNId = MxNId;
  MxNId++;
  if (!NodeH.IsKey(ModeId)) {
    NodeH.AddDat(ModeId);
  }
  NodeH.GetDat(ModeId).AddDat(LocalNId,TNode(LocalNId));
  return TPair<TInt,TInt>(ModeId,LocalNId);
}

void TMultimodalGraphImplC::DelNode(TPair<TInt,TInt> NId) {
  int LocalNId = NId.GetVal2();
  int ModeId = NId.GetVal1();
  IAssertR(IsNode(NId), TStr::Fmt("NodeId (%d,%d) doesn't exist", NId.GetVal1(), NId.GetVal2()));

  TNode& Node = NodeH.GetDat(ModeId).GetDat(LocalNId);
  for (int e = 0; e < Node.GetOutDeg(); e++) {
    const TPair<TInt,TInt> nbr = Node.GetOutNId(e);
    if (nbr == NId) { continue; }
    TNode& N = NodeH.GetDat(nbr.GetVal1()).GetDat(nbr.GetVal2());
    const int n = N.InNIdV.SearchBin(NId);
    if (n!= -1) { N.InNIdV.Del(n); NEdges--; }
  }
  for (int e = 0; e < Node.GetInDeg(); e++) {
    const TPair<TInt,TInt> nbr = Node.GetInNId(e);
    if (nbr == NId) { continue; }
    TNode& N = NodeH.GetDat(nbr.GetVal1()).GetDat(nbr.GetVal2());
    const int n = N.OutNIdV.SearchBin(NId);
    if (n!= -1) { N.OutNIdV.Del(n); NEdges--; }
  }
  NodeH.GetDat(ModeId).DelKey(LocalNId);
}

bool TMultimodalGraphImplC::IsNode(TPair<TInt,TInt> NId) {
  int ModeId = NId.GetVal1();
  int LocalNId = NId.GetVal2();
  if (!NodeH.IsKey(ModeId)) {
    return false;
  }
  if (!NodeH.GetDat(ModeId).IsKey(LocalNId)) {
    return false;
  }
  return true;
}

TMultimodalGraphImplC::TNodeI TMultimodalGraphImplC::GetNI(const TPair<TInt,TInt>& NId) const {
  int ModeId = NId.GetVal1();
  int LocalNId = NId.GetVal2();

  return TNodeI(NodeH.GetI(ModeId), NodeH.EndI(), NodeH.GetDat(ModeId).GetI(LocalNId));
}

void TMultimodalGraphImplC::GetNodeIdsInMode(const int ModeId, TVec< TPair<TInt,TInt> >& NodeIds) const {
  const THash<TInt,TNode>& PerNodeHash = NodeH.GetDat(ModeId);
  for (THash<TInt,TNode>::TIter NI=PerNodeHash.BegI(); NI<PerNodeHash.EndI(); NI++) {
    NodeIds.Add(TPair<TInt,TInt>(ModeId, NI.GetKey()));
  }
}

int TMultimodalGraphImplC::AddEdge(const TPair<TInt,TInt>& SrcNId, const TPair<TInt,TInt>& DstNId) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("(%d,%d) or (%d,%d) not a node.", SrcNId.GetVal1(), SrcNId.GetVal2(), DstNId.GetVal1(), DstNId.GetVal2()).CStr());
  if (IsEdge(SrcNId, DstNId)) {
    return -2; // Edge already exists
  }
  NEdges++;

  int SrcModeId = SrcNId.GetVal1();
  int SrcLocalNId = SrcNId.GetVal2();
  int DstModeId = DstNId.GetVal1();
  int DstLocalNId = DstNId.GetVal2();

  NodeH.GetDat(SrcModeId).GetDat(SrcLocalNId).OutNIdV.AddSorted(DstNId);
  NodeH.GetDat(DstModeId).GetDat(DstLocalNId).InNIdV.AddSorted(SrcNId);
  return -1;
}

void TMultimodalGraphImplC::AddEdgeBatch(const TPair<TInt,TInt>& SrcNId, const TVec<TPair<TInt,TInt> >& DstNIds) {
  IAssertR(IsNode(SrcNId), TStr::Fmt("(%d,%d) not a node.", SrcNId.GetVal1(), SrcNId.GetVal2()).CStr());
  ReserveOutNIdV(SrcNId, DstNIds.Len());
  for (TVec<TPair<TInt,TInt> >::TIter DstNId = DstNIds.BegI(); DstNId < DstNIds.EndI(); DstNId++) {
    AddEdge(SrcNId, *DstNId);
  }
}

void TMultimodalGraphImplC::DelEdge(const TPair<TInt,TInt>& SrcNId, const TPair<TInt,TInt>& DstNId) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("(%d,%s) or (%d,%d) not a node.", SrcNId.GetVal1(), SrcNId.GetVal2(), DstNId.GetVal1(), DstNId.GetVal2()).CStr());
  if (!IsEdge(SrcNId, DstNId)) {
    return; // Edge doesn't exist
  }
  NEdges--;

  int SrcModeId = SrcNId.GetVal1();
  int SrcLocalNId = SrcNId.GetVal2();
  int DstModeId = DstNId.GetVal1();
  int DstLocalNId = DstNId.GetVal2();

  NodeH.GetDat(SrcModeId).GetDat(SrcLocalNId).OutNIdV.DelIfInSorted(DstNId);
  NodeH.GetDat(DstModeId).GetDat(DstLocalNId).InNIdV.DelIfInSorted(SrcNId);
}

bool TMultimodalGraphImplC::IsEdge(const TPair<TInt,TInt>& SrcNId, const TPair<TInt,TInt>& DstNId) {
  if (! IsNode(SrcNId) || !IsNode(DstNId)) { return false; }
  int SrcModeId = SrcNId.GetVal1();
  int SrcLocalNId = SrcNId.GetVal2();
  return NodeH.GetDat(SrcModeId).GetDat(SrcLocalNId).IsOutNId(DstNId);
}

TMultimodalGraphImplC::TEdgeI TMultimodalGraphImplC::GetEI(const TPair<TInt,TInt>& SrcNId, const TPair<TInt,TInt>& DstNId) const {
  const TNodeI SrcNI = GetNI(SrcNId);
  const int NodeN = SrcNI.NodeNHI.GetDat().OutNIdV.SearchBin(DstNId);
  if (NodeN == -1) { return EndEI(); }
  return TEdgeI(SrcNI, EndNI(), NodeN);
}

TIntNNet TMultimodalGraphImplC::GetSubGraph(const TIntV ModeIds) const {
  TIntNNet SubGraph = TIntNNet();

  for (int ModeIdx = 0; ModeIdx < ModeIds.Len(); ModeIdx++) {
    int ModeId = ModeIds.GetVal(ModeIdx);
    if (!NodeH.IsKey(ModeId)) { continue; }
    for (THash<TInt, TNode>::TIter NI=NodeH.GetDat(ModeId).BegI(); NI<NodeH.GetDat(ModeId).EndI(); NI++) {
      int NId = NI.GetDat().GetId();
      SubGraph.AddNode(NId, ModeId);
    }
  }

  for (int ModeIdx = 0; ModeIdx < ModeIds.Len(); ModeIdx++) {
    int ModeId = ModeIds.GetVal(ModeIdx);
    if (!NodeH.IsKey(ModeId)) { continue; }
    for (THash<TInt, TNode>::TIter NI=NodeH.GetDat(ModeId).BegI(); NI<NodeH.GetDat(ModeId).EndI(); NI++) {
      int NId = NI.GetDat().GetId();
      for (int e = 0; e < NI.GetDat().GetOutDeg(); e++) {
        TPair<TInt,TInt> NeighboringNId = NI.GetDat().GetOutNId(e);
        if (ModeIds.IsIn(NeighboringNId.GetVal1())) {
          SubGraph.AddEdge(NId, NeighboringNId.GetVal2());
        }
      }
    }
  }
  printf("Number of nodes in SubGraph: %d...\n", SubGraph.GetNodes());
  printf("Number of edges in SubGraph: %d...\n", SubGraph.GetEdges());

  return SubGraph;
}

int TMultimodalGraphImplC::GetSubGraphMocked(const TIntV ModeIds) const {
  int NumVerticesAndEdges = 0;

  for (int ModeIdx = 0; ModeIdx < ModeIds.Len(); ModeIdx++) {
    int ModeId = ModeIds.GetVal(ModeIdx);
    if (!NodeH.IsKey(ModeId)) { continue; }
    for (THash<TInt, TNode>::TIter NI=NodeH.GetDat(ModeId).BegI(); NI<NodeH.GetDat(ModeId).EndI(); NI++) {
      NumVerticesAndEdges++;
    }
  }

  for (int ModeIdx = 0; ModeIdx < ModeIds.Len(); ModeIdx++) {
    int ModeId = ModeIds.GetVal(ModeIdx);
    if (!NodeH.IsKey(ModeId)) { continue; }
    for (THash<TInt, TNode>::TIter NI=NodeH.GetDat(ModeId).BegI(); NI<NodeH.GetDat(ModeId).EndI(); NI++) {
      for (int e = 0; e < NI.GetDat().GetOutDeg(); e++) {
        TPair<TInt,TInt> NeighboringNId = NI.GetDat().GetOutNId(e);
        if (ModeIds.IsIn(NeighboringNId.GetVal1())) {
          NumVerticesAndEdges += NeighboringNId.GetVal2();
        }
      }
    }
  }

  return NumVerticesAndEdges;
}

PMultimodalGraphImplC TMultimodalGraphImplC::GetSmallGraph() {
  PMultimodalGraphImplC G = TMultimodalGraphImplC::New();
  TVec< TPair<TInt,TInt> > Nodes = TVec< TPair<TInt,TInt> >();
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

