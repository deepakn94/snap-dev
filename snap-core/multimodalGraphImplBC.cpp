/////////////////////////////////////////////////
// Multi-modal Graph, Impl C

int TMultimodalGraphImplBC::TNodeI::GetInDeg() const {
  int InDeg = 0;
  const THash<TInt, TVec<TInt> >& InNIdV = NodeNHI.GetDat().InNIdV;
  for (THash<TInt, TVec<TInt> >::TIter N=InNIdV.BegI(); N<InNIdV.EndI(); N++) {
    InDeg += N.GetDat().Len();
  }
  return InDeg;
}

int TMultimodalGraphImplBC::TNodeI::GetOutDeg() const {
  int OutDeg = 0;
  const THash<TInt, TVec<TInt> >& OutNIdV = NodeNHI.GetDat().OutNIdV;
  for (THash<TInt, TVec<TInt> >::TIter N=OutNIdV.BegI(); N<OutNIdV.EndI(); N++) {
    OutDeg += N.GetDat().Len();
  }
  return OutDeg;
}

int TMultimodalGraphImplBC::GetNodes() const {
  int NumNodes = 0;
  for (TNodeH::TIter Node=NodeH.BegI(); Node<NodeH.EndI(); Node++) {
    NumNodes += Node.GetDat().Len();
  }
  return NumNodes;
}

void TMultimodalGraphImplBC::TNodeI::GetAdjacentModes(TIntV &AdjacentModes) const {
  const THash<TInt, TVec<TInt> >& OutNIdV = NodeNHI.GetDat().OutNIdV;
  for (THash<TInt, TVec<TInt> >::TIter N=OutNIdV.BegI(); N<OutNIdV.EndI(); N++) {
    AdjacentModes.Add(N.GetKey());
  }

  const THash<TInt, TVec<TInt> >& InNIdV = NodeNHI.GetDat().InNIdV;
  for (THash<TInt, TVec<TInt> >::TIter N=InNIdV.BegI(); N<InNIdV.EndI(); N++) {
    AdjacentModes.Add(N.GetKey());
  }
}

TPair<TInt,TInt> TMultimodalGraphImplBC::AddNode(int ModeId) {
  int LocalNId = MxNId;
  MxNId++;
  if (!NodeH.IsKey(ModeId)) {
    NodeH.AddDat(ModeId);
  }
  NodeH.GetDat(ModeId).AddDat(LocalNId,TNode(LocalNId));
  return TPair<TInt,TInt>(ModeId,LocalNId);
}

void TMultimodalGraphImplBC::AddNode(const TPair<TInt,TInt>& NodeId) {
  int ModeId = NodeId.GetVal1(); int LocalNId = NodeId.GetVal2();
  if (!NodeH.IsKey(ModeId)) {
    NodeH.AddDat(ModeId);
  }
  NodeH.GetDat(ModeId).AddDat(LocalNId,TNode(LocalNId));
}

void TMultimodalGraphImplBC::DelNode(TPair<TInt,TInt> NId) {
  int LocalNId = NId.GetVal2();
  int ModeId = NId.GetVal1();
  IAssertR(IsNode(NId), TStr::Fmt("NodeId (%d,%d) doesn't exist", NId.GetVal1(), NId.GetVal2()));

  TNode& Node = NodeH.GetDat(ModeId).GetDat(LocalNId);
  for (THash<TInt, TVec<TInt> >::TIter N=Node.OutNIdV.BegI(); N<Node.OutNIdV.EndI(); N++) {
    TVec<TInt>& OutNIdV = N.GetDat();
    for (int e = 0; e < OutNIdV.Len(); e++) {
      TNode& Node2 = NodeH.GetDat(N.GetKey()).GetDat(OutNIdV.GetVal(e));
      const int n = Node2.InNIdV.GetDat(NId.GetVal1()).SearchBin(NId.GetVal2());
      if (n!= -1) { Node2.InNIdV.GetDat(NId.GetVal1()).Del(n); NEdges--; }
    }
  }
  for (THash<TInt, TVec<TInt> >::TIter N=Node.InNIdV.BegI(); N<Node.InNIdV.EndI(); N++) {
    TVec<TInt>& InNIdV = N.GetDat();
    for (int e = 0; e < InNIdV.Len(); e++) {
      TNode& Node2 = NodeH.GetDat(N.GetKey()).GetDat(InNIdV.GetVal(e));
      const int n = Node2.OutNIdV.GetDat(NId.GetVal1()).SearchBin(NId.GetVal2());
      if (n!= -1) { Node2.OutNIdV.GetDat(NId.GetVal1()).Del(n); NEdges--; }
    }
  }
  NodeH.GetDat(ModeId).DelKey(LocalNId);
}

bool TMultimodalGraphImplBC::IsNode(TPair<TInt,TInt> NId) {
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

TMultimodalGraphImplBC::TNodeI TMultimodalGraphImplBC::GetNI(const TPair<TInt,TInt>& NId) const {
  int ModeId = NId.GetVal1();
  int LocalNId = NId.GetVal2();

  return TNodeI(NodeH.GetI(ModeId), NodeH.EndI(), NodeH.GetDat(ModeId).GetI(LocalNId));
}

void TMultimodalGraphImplBC::GetNodeIdsInMode(const int ModeId, TVec< TPair<TInt,TInt> >& NodeIds) const {
  const THash<TInt,TNode>& PerNodeHash = NodeH.GetDat(ModeId);
  for (THash<TInt,TNode>::TIter NI=PerNodeHash.BegI(); NI<PerNodeHash.EndI(); NI++) {
    NodeIds.Add(TPair<TInt,TInt>(ModeId, NI.GetKey()));
  }
}

int TMultimodalGraphImplBC::AddEdge(const TPair<TInt,TInt>& SrcNId, const TPair<TInt,TInt>& DstNId) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("(%d,%d) or (%d,%d) not a node.", SrcNId.GetVal1(), SrcNId.GetVal2(), DstNId.GetVal1(), DstNId.GetVal2()).CStr());
  if (IsEdge(SrcNId, DstNId)) {
    return -2; // Edge already exists
  }
  NEdges++;

  int SrcModeId = SrcNId.GetVal1();
  int SrcLocalNId = SrcNId.GetVal2();
  int DstModeId = DstNId.GetVal1();
  int DstLocalNId = DstNId.GetVal2();

  THash<TInt, TVec<TInt> >& OutNIdV = NodeH.GetDat(SrcModeId).GetDat(SrcLocalNId).OutNIdV;
  THash<TInt, TVec<TInt> >& InNIdV = NodeH.GetDat(DstModeId).GetDat(DstLocalNId).InNIdV;

  if (!OutNIdV.IsKey(DstModeId)) { OutNIdV.AddDat(DstModeId); }
  if (!InNIdV.IsKey(SrcModeId)) { InNIdV.AddDat(SrcModeId); }

  OutNIdV.GetDat(DstModeId).AddSorted(DstLocalNId);
  InNIdV.GetDat(SrcModeId).AddSorted(SrcLocalNId);

  return -1;
}

void TMultimodalGraphImplBC::AddEdgeBatch(const TPair<TInt,TInt>& SrcNId, const TVec<TPair<TInt,TInt> >& DstNIds) {
  IAssertR(IsNode(SrcNId), TStr::Fmt("(%d,%d) not a node.", SrcNId.GetVal1(), SrcNId.GetVal2()).CStr());
  for (TVec<TPair<TInt,TInt> >::TIter DstNId = DstNIds.BegI(); DstNId < DstNIds.EndI(); DstNId++) {
    AddEdge(SrcNId, *DstNId);
  }
}

void TMultimodalGraphImplBC::DelEdge(const TPair<TInt,TInt>& SrcNId, const TPair<TInt,TInt>& DstNId) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("(%d,%s) or (%d,%d) not a node.", SrcNId.GetVal1(), SrcNId.GetVal2(), DstNId.GetVal1(), DstNId.GetVal2()).CStr());
  if (!IsEdge(SrcNId, DstNId)) {
    return; // Edge doesn't exist
  }
  NEdges--;

  int SrcModeId = SrcNId.GetVal1();
  int SrcLocalNId = SrcNId.GetVal2();
  int DstModeId = DstNId.GetVal1();
  int DstLocalNId = DstNId.GetVal2();

  NodeH.GetDat(SrcModeId).GetDat(SrcLocalNId).OutNIdV.GetDat(DstModeId).DelIfInSorted(DstLocalNId);
  NodeH.GetDat(DstModeId).GetDat(DstLocalNId).InNIdV.GetDat(SrcModeId).DelIfInSorted(SrcLocalNId);
}

bool TMultimodalGraphImplBC::IsEdge(const TPair<TInt,TInt>& SrcNId, const TPair<TInt,TInt>& DstNId) {
  if (! IsNode(SrcNId) || !IsNode(DstNId)) { return false; }
  int SrcModeId = SrcNId.GetVal1();
  int SrcLocalNId = SrcNId.GetVal2();
  return NodeH.GetDat(SrcModeId).GetDat(SrcLocalNId).IsOutNId(DstNId);
}

TMultimodalGraphImplBC::TEdgeI TMultimodalGraphImplBC::GetEI(const TPair<TInt,TInt>& SrcNId, const TPair<TInt,TInt>& DstNId) const {
  const TNodeI SrcNI = GetNI(SrcNId);
  const int NodeN = SrcNI.NodeNHI.GetDat().OutNIdV.GetDat(DstNId.GetVal1()).SearchBin(DstNId.GetVal2());
  if (NodeN == -1) { return EndEI(); }
  return TEdgeI(SrcNI, EndNI(), NodeN);
}

TIntNNet TMultimodalGraphImplBC::GetSubGraph(const TIntV ModeIds) const {
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
      for (int ModeIdx2 = 0; ModeIdx2 < ModeIds.Len(); ModeIdx2++) {
        int ModeId2 = ModeIds.GetVal(ModeIdx2);
        for (int e = 0; e < NI.GetDat().GetOutDeg(ModeId2); e++) {
          SubGraph.AddEdge(NId, NI.GetDat().GetOutNId(e, ModeId2));
        }
      }
    }
  }
  printf("Number of nodes in SubGraph: %d...\n", SubGraph.GetNodes());
  printf("Number of edges in SubGraph: %d...\n", SubGraph.GetEdges());

  return SubGraph;
}

int TMultimodalGraphImplBC::GetSubGraphMocked(const TIntV ModeIds) const {
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
      for (int ModeIdx2 = 0; ModeIdx2 < ModeIds.Len(); ModeIdx2++) {
        int ModeId2 = ModeIds.GetVal(ModeIdx2);
        for (int e = 0; e < NI.GetDat().GetOutDeg(ModeId2); e++) {
          NumVerticesAndEdges += NI.GetDat().GetOutNId(e, ModeId2);
        }
      }
    }
  }

  return NumVerticesAndEdges;
}

int TMultimodalGraphImplBC::BFSTraversalOneHop(const TVec< TPair<TInt,TInt> >& StartingVertices) const {
  int NumVerticesAndEdges = 0;
  for (int i = 0; i < StartingVertices.Len(); i++) {
    TNodeI NI = GetNI(StartingVertices.GetVal(i));
    TIntV AdjacentModes = TIntV(); NI.GetAdjacentModes(AdjacentModes);
    for (int ModeIdx = 0; ModeIdx < AdjacentModes.Len(); ModeIdx++) {
      int ModeId = AdjacentModes.GetVal(ModeIdx);
      for (int e = 0; e < NI.GetOutDeg(ModeId); e++) {
        NumVerticesAndEdges += NI.GetOutNId(e, ModeId);
      }
    }
  }
  return NumVerticesAndEdges;
}

PMultimodalGraphImplBC TMultimodalGraphImplBC::GetSmallGraph() {
  PMultimodalGraphImplBC G = TMultimodalGraphImplBC::New();
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

