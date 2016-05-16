/////////////////////////////////////////////////
// Multi-modal Graph, Impl B

int TMultimodalGraphImplB::TNodeI::GetInDeg() const {
  int ModeId = NodeToModeMapping->GetDat(GetId());
  int InDeg = 0;
  for (TGraphs::TIter it = Graphs->BegI(); it < Graphs->EndI(); it++) {
    if (it.GetKey().GetVal1() == ModeId || it.GetKey().GetVal2() == ModeId) {
      if (it.GetDat().IsNode(GetId())) {
        InDeg += it.GetDat().GetNI(GetId()).GetInDeg();
      }
    }
  }
  return InDeg;
}

int TMultimodalGraphImplB::TNodeI::GetInDeg(const int &ModeId) const {
  int CurModeId = NodeToModeMapping->GetDat(GetId());
  TPair<TInt,TInt> ModeIdsKey = GetModeIdsKey(CurModeId, ModeId);
  if (Graphs->GetDat(ModeIdsKey).IsNode(GetId()))
    return Graphs->GetDat(ModeIdsKey).GetNI(GetId()).GetInDeg();
  return 0;
}

int TMultimodalGraphImplB::TNodeI::GetOutDeg() const {
  int ModeId = NodeToModeMapping->GetDat(GetId());
  int OutDeg = 0;
  for (TGraphs::TIter it = Graphs->BegI(); it < Graphs->EndI(); it++) {
    if (it.GetKey().GetVal1() == ModeId || it.GetKey().GetVal2() == ModeId) {
      if (it.GetDat().IsNode(GetId())) {
        OutDeg += it.GetDat().GetNI(GetId()).GetOutDeg();
      }
    }
  }
  return OutDeg;
}

int TMultimodalGraphImplB::TNodeI::GetOutDeg(const int &ModeId) const {
  int CurModeId = NodeToModeMapping->GetDat(GetId());
  TPair<TInt,TInt> ModeIdsKey = GetModeIdsKey(CurModeId, ModeId);
  if (Graphs->GetDat(ModeIdsKey).IsNode(GetId()))
    return Graphs->GetDat(ModeIdsKey).GetNI(GetId()).GetOutDeg();
  return 0;
}

void TMultimodalGraphImplB::TNodeI::GetAdjacentModes(TIntV &AdjacentModes) const {
  int ModeId = NodeToModeMapping->GetDat(GetId());
  for (TGraphs::TIter it = Graphs->BegI(); it < Graphs->EndI(); it++) {
    if (it.GetKey().GetVal1() == ModeId) {
      AdjacentModes.Add(it.GetKey().GetVal2());
    } else if (it.GetKey().GetVal2() == ModeId) {
      AdjacentModes.Add(it.GetKey().GetVal1());
    }
  }
}

void TMultimodalGraphImplB::TNodeI::SortNIdV() {
  int ModeId = NodeToModeMapping->GetDat(GetId());
  for (TGraphs::TIter it = Graphs->BegI(); it < Graphs->EndI(); it++) {
    if (it.GetKey().GetVal1() == ModeId || it.GetKey().GetVal2() == ModeId) {
      if (it.GetDat().IsNode(GetId())) {
        it.GetDat().GetNI(GetId()).SortNIdV();
      }
    }
  }
}

TPair<TInt,TInt> TMultimodalGraphImplB::AddNode(int ModeId) {
  int LocalNId = MxNId;
  MxNId++;
  NodeToModeMapping.AddDat(LocalNId, ModeId);
  return TPair<TInt,TInt>(ModeId,LocalNId);
}

void TMultimodalGraphImplB::AddNode(const TPair<TInt,TInt>& NodeId) {
  NodeToModeMapping.AddDat(NodeId.GetVal2(), NodeId.GetVal1());
}

void TMultimodalGraphImplB::DelNode(const TPair<TInt,TInt>& NId) {
  IAssertR(IsNode(NId), TStr::Fmt("NodeId %d does not exist", NId.GetVal2()));
  int ModeId = NId.GetVal1();
  // Remove NId from all relevant graphs
  for (TGraphs::TIter it = Graphs.BegI(); it < Graphs.EndI(); it++) {
    TPair<TInt, TInt> ModeIdPair = it.GetKey();
    if (ModeIdPair.GetVal1() == ModeId || ModeIdPair.GetVal2() == ModeId) {
      TNGraph& Graph = it.GetDat();
      if (Graph.IsNode(NId.GetVal2())) {
        Graph.DelNode(NId.GetVal2());
      }
    }
  }
  // Remove NId from NodeToModeMapping as well
  NodeToModeMapping.DelKey(NId.GetVal2());
}

void TMultimodalGraphImplB::GetNodeIdsInMode(const int ModeId, TVec< TPair<TInt,TInt> >& NodeIds) const {
  for (THash<TInt,TInt>::TIter NI=NodeToModeMapping.BegI(); NI<NodeToModeMapping.EndI(); NI++) {
    if (NI.GetDat() == ModeId) {
      NodeIds.Add(TPair<TInt,TInt>(NI.GetDat(), NI.GetKey()));
    }
  }
}

int TMultimodalGraphImplB::AddEdge(const TPair<TInt,TInt>& SrcNId, const TPair<TInt,TInt>& DstNId) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId.GetVal2(), DstNId.GetVal2()).CStr());

  TPair<TInt,TInt> ModeIdsKey = GetModeIdsKey(SrcNId.GetVal1(), DstNId.GetVal1());
  if (!Graphs.IsKey(ModeIdsKey)) {
    Graphs.AddDat(ModeIdsKey, TNGraph());
  }

  if (!Graphs.GetDat(ModeIdsKey).IsNode(SrcNId.GetVal2())) {
    Graphs.GetDat(ModeIdsKey).AddNode(SrcNId.GetVal2());
  }
  if (!Graphs.GetDat(ModeIdsKey).IsNode(DstNId.GetVal2())) {
    Graphs.GetDat(ModeIdsKey).AddNode(DstNId.GetVal2());
  }

  if (Graphs.GetDat(ModeIdsKey).IsEdge(SrcNId.GetVal2(), DstNId.GetVal2())) {
    return -2; // Edge already exists
  }
  NEdges++;
  return Graphs.GetDat(ModeIdsKey).AddEdge(SrcNId.GetVal2(), DstNId.GetVal2());
}

void TMultimodalGraphImplB::AddEdgeBatch(const TPair<TInt,TInt>& SrcNId, const TVec<TPair<TInt,TInt> >& DstNIds) {
  IAssertR(IsNode(SrcNId), TStr::Fmt("%d not a node.", SrcNId.GetVal2()).CStr());
  for (TVec<TPair<TInt,TInt> >::TIter DstNId = DstNIds.BegI(); DstNId < DstNIds.EndI(); DstNId++) {
    AddEdge(SrcNId, *DstNId);
  }
}

bool TMultimodalGraphImplB::IsEdge(const TPair<TInt,TInt>& SrcNId, const TPair<TInt,TInt>& DstNId) const {
  if (!IsNode(SrcNId) || !IsNode(DstNId)) return false;

  TPair<TInt,TInt> ModeIdsKey = GetModeIdsKey(SrcNId.GetVal1(), DstNId.GetVal2());
  if (!Graphs.IsKey(ModeIdsKey)) {
    return false;
  }

  return Graphs.GetDat(ModeIdsKey).IsEdge(SrcNId.GetVal2(), DstNId.GetVal2());
}

void TMultimodalGraphImplB::DelEdge(const TPair<TInt,TInt>& SrcNId, const TPair<TInt,TInt>& DstNId) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId.GetVal2(), DstNId.GetVal2()).CStr());
  if (!IsEdge(SrcNId, DstNId)) {
    return; // Edge doesn't exist
  }
  NEdges--;

  TPair<TInt,TInt> ModeIdsKey = GetModeIdsKey(SrcNId.GetVal1(), DstNId.GetVal1());
  Graphs.GetDat(ModeIdsKey).DelEdge(SrcNId.GetVal2(), DstNId.GetVal2());
}

TMultimodalGraphImplB::TNodeI TMultimodalGraphImplB::BegNI() const {
  return TNodeI(NodeToModeMapping.BegI(), &Graphs, &NodeToModeMapping);
}

TMultimodalGraphImplB::TNodeI TMultimodalGraphImplB::EndNI() const {
  return TNodeI(NodeToModeMapping.EndI(), &Graphs, &NodeToModeMapping);
}

TMultimodalGraphImplB::TNodeI TMultimodalGraphImplB::GetNI(const TPair<TInt,TInt>& NId) const {
  return TNodeI(NodeToModeMapping.GetI(NId.GetVal2()), &Graphs, &NodeToModeMapping);
}

TMultimodalGraphImplB::TEdgeI TMultimodalGraphImplB::GetEI(const TPair<TInt,TInt>& SrcNId, const TPair<TInt,TInt>& DstNId) const {
  TNodeI CurNode = TNodeI(NodeToModeMapping.GetI(SrcNId.GetVal2()), &Graphs, &NodeToModeMapping);
  TIntV AdjacentModes = TIntV();
  CurNode.GetAdjacentModes(AdjacentModes);

  TPair<TInt,TInt> ModeIdsKey = GetModeIdsKey(SrcNId.GetVal1(), DstNId.GetVal1());

  int CurAdjacentMode = 0;
  for (int i=0; i<AdjacentModes.Len();i++) {
    if (AdjacentModes.GetDat(i) == DstNId.GetVal1()) {
      CurAdjacentMode=i;
      break;
    }
  }

  return TEdgeI(CurNode, EndNI(), Graphs.GetDat(ModeIdsKey).GetEI(SrcNId.GetVal2(), DstNId.GetVal2()).GetCurEdge(), AdjacentModes, CurAdjacentMode);
}

void TMultimodalGraphImplB::GetNIdV(TIntV& NIdV) const {
  NIdV.Gen(GetNodes(), 0);
  for (int N=NodeToModeMapping.FFirstKeyId(); NodeToModeMapping.FNextKeyId(N); ) {
    NIdV.Add(NodeToModeMapping.GetKey(N));
  }
}

void TMultimodalGraphImplB::Clr() {
  MxNId = 0;
  NEdges = 0;
  NodeToModeMapping.Clr();
  for (TGraphs::TIter it = Graphs.BegI(); it < Graphs.EndI(); it++) {
    it.GetDat().Clr();
  }
  Graphs.Clr();
}

void TMultimodalGraphImplB::Defrag(const bool& OnlyNodeLinks) {
  for (TGraphs::TIter it = Graphs.BegI(); it < Graphs.EndI(); it++) {
    it.GetDat().Defrag(OnlyNodeLinks);
  }
}

bool TMultimodalGraphImplB::IsOk(const bool& ThrowExcept) const {
  for (TGraphs::TIter it = Graphs.BegI(); it < Graphs.EndI(); it++) {
    if (!it.GetDat().IsOk(ThrowExcept)) {
      return false;
    }
  }
  return true;
}

TIntNNet TMultimodalGraphImplB::GetSubGraph(const TIntV ModeIds) const {
  TIntNNet SubGraph = TIntNNet();

  for (THash<TInt,TInt>::TIter CurI = NodeToModeMapping.BegI(); CurI < NodeToModeMapping.EndI(); CurI++) {
    if (ModeIds.IsIn(CurI.GetDat())) {
      SubGraph.AddNode(CurI.GetKey(), CurI.GetDat());
    }
  }

  for (int ModeIdx1 = 0; ModeIdx1 < ModeIds.Len(); ModeIdx1++) {
    int ModeId1 = ModeIds.GetVal(ModeIdx1);
    for (int ModeIdx2 = 0; ModeIdx2 < ModeIds.Len(); ModeIdx2++) {
      int ModeId2 = ModeIds.GetVal(ModeIdx2);
      TPair<TInt,TInt> ModeIdsKey = GetModeIdsKey(ModeId1, ModeId2);
      if (!Graphs.IsKey(ModeIdsKey)) { continue; }
      const TNGraph& Graph = Graphs.GetDat(ModeIdsKey);
      for (TNGraph::TNodeI it = Graph.BegNI(); it < Graph.EndNI(); it++) {
        for (int e = 0; e < it.GetOutDeg(); e++) {
          SubGraph.AddEdge(it.GetId(), it.GetOutNId(e));
        }
      }
    }
  }
  printf("Number of nodes in SubGraph: %d...\n", SubGraph.GetNodes());
  printf("Number of edges in SubGraph: %d...\n", SubGraph.GetEdges());

  return SubGraph;
}

int TMultimodalGraphImplB::GetSubGraphMocked(const TIntV ModeIds) const {
  int NumVerticesAndEdges = 0;

  for (THash<TInt,TInt>::TIter CurI = NodeToModeMapping.BegI(); CurI < NodeToModeMapping.EndI(); CurI++) {
    if (ModeIds.IsIn(CurI.GetDat())) {
      NumVerticesAndEdges++;
    }
  }

  for (int ModeIdx1 = 0; ModeIdx1 < ModeIds.Len(); ModeIdx1++) {
    int ModeId1 = ModeIds.GetVal(ModeIdx1);
    for (int ModeIdx2 = 0; ModeIdx2 < ModeIds.Len(); ModeIdx2++) {
      int ModeId2 = ModeIds.GetVal(ModeIdx2);
      TPair<TInt,TInt> ModeIdsKey = GetModeIdsKey(ModeId1, ModeId2);
      if (!Graphs.IsKey(ModeIdsKey)) { continue; }
      const TNGraph& Graph = Graphs.GetDat(ModeIdsKey);
      for (TNGraph::TNodeI it = Graph.BegNI(); it < Graph.EndNI(); it++) {
        for (int e = 0; e < it.GetOutDeg(); e++) {
          NumVerticesAndEdges += it.GetOutNId(e);
        }
      }
    }
  }

  return NumVerticesAndEdges;
}

PMultimodalGraphImplB TMultimodalGraphImplB::GetSmallGraph() {
  PMultimodalGraphImplB G = TMultimodalGraphImplB::New();
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

