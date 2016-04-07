/////////////////////////////////////////////////
// Multi-modal Graph, Impl B

int TMultimodalGraphImplB::TNodeI::GetInDeg() const {
  int ModeId = NodeToModeMapping->GetDat(GetId());
  int InDeg = 0;
  for (TGraphs::TIter it = Graphs.BegI(); it < Graphs.EndI(); it++) {
    if (it.GetKey().GetVal1() == ModeId || it.GetKey().GetVal2() == ModeId) {
      if (it.GetDat().IsNode(GetId())) {
        InDeg += it.GetDat().GetNI(GetId()).GetInDeg();
      }
    }
  }
  return InDeg;
}

int MultimodalGraphImplB::TNodeI::GetOutDeg() const {
  int ModeId = NodeToModeMapping->GetDat(GetId());
  int OutDeg = 0;
  for (TGraphs::TIter it = Graphs.BegI(); it < Graphs.EndI(); it++) {
    if (it.GetKey().GetVal1() == ModeId || it.GetKey().GetVal2() == ModeId) {
      if (it.GetDat().IsNode(GetId())) {
        OutDeg += it.GetDat().GetNI(GetId()).GetOutDeg();
      }
    }
  }
  return OutDeg;
}

void MultimodalGraphImplB::TNodeI::GetAdjacentModes(TIntV &AdjacentModes) const {
  int ModeId = NodeToModeMapping[GetId()];
  for (TGraphs::TIter it = Graphs.BegI(); it < Graphs.EndI(); it++) {
    if (it.GetKey().GetVal1() == ModeId) {
      AdjacentModes.Add(it.GetKey().GetVal2());
    } else if (it.GetKey().GetVal2() == ModeId) {
      AdjacentModes.Add(it.GetKey().GetVal1());
    }
  }
}

void MultimodalGraphImplB::TNodeI::SortNIdV() {
  int ModeId = NodeToModeMapping[GetId()];
  for (TGraphs::TIter it = Graphs.BegI(); it < Graphs.EndI(); it++) {
    if (it.GetKey().GetVal1() == ModeId || it.GetKey().GetVal2() == ModeId) {
      if (it.GetDat().IsNode(GetId())) {
        it.GetDat().GetNI(GetId()).SortNIdV();
      }
    }
  }
}

int TMultimodalGraphImplB::AddNode(int NId, int ModeId) {
  if (NId == -1) {
    NId = MxNId;  MxNId++;
  } else {
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
    MxNId = TMath::Mx(NId+1, MxNId());
  }
  // Insert NId->ModeId mapping as appropriate
  NodeToModeMapping.AddDat(NId, ModeId);
  return NId;
}

int TMultimodalGraphImplB::DelNode(int NId) {
  IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d does not exist", NId));
  int ModeId = NodeToModeMapping[NId];
  // Remove NId from all relevant graphs
  for (TGraphs::TIter it = Graphs.BegI(); it < Graphs.EndI(); it++) {
    TPair<TInt, TInt> ModeIdPair = it.GetKey();
    if (ModeIdPair.GetVal1() == ModeId || ModeIdPair.GetVal2() == ModeId) {
      it.GetDat().DelNode(NId);
    }
  }
  // Remove NId from NodeToModeMapping as well
  NodeToModeMapping.DelKey(NId);
} 

int TMultimodalGraphImplB::AddEdge(const int& SrcNId, const int& DstNId) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());

  TPair<TInt,TInt> ModeIdsKey = GetModeIdsKey(NodeToModeMapping.GetDat(SrcNId), NodeToModeMapping.GetDat(DstNId));
  if (!Graphs.IsKey(ModeIdsKey)) {
    Graphs.AddDat(ModeIdsKey, TNGraph());
  }

  if (Graphs[ModeIdsKey].IsEdge(SrcNId, DstNId)) {
    return -2; // Edge already exists
  }
  NEdges++;
  return Graphs[ModeIdsKey].AddEdge(SrcNId, DstNId);
}

bool TMultimodalGraphImplB::IsEdge(const int& SrcNId, const int& DstNId) {
  if (!IsNode(SrcNId) || !IsNode(DstNId)) return false;

  TPair<TInt,TInt> ModeIdsKey = GetModeIdsKey(NodeToModeMapping.GetDat(SrcNId), NodeToModeMapping.GetDat(DstNId));
  if (!Graphs.IsKey(ModsIdsKey)) {
    return false;
  }

  return Graphs[ModeIdsKey].IsEdge(SrcNId, DstNId);
}

void TMultimodalGraphImplB::DelEdge(const int& SrcNId, const int& DstNId) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
  if (!IsEdge(SrcNId, DstNId)) {
    return; // Edge doesn't exist
  }
  NEdges--;

  TPair<TInt,TInt> ModeIdsKey = GetModeIdsKey(NodeToModeMapping.GetDat(SrcNId), NodeToModeMapping.GetDat(DstNId));
  Graphs[ModeIdsKey].DelEdge(SrcNId, DstNId);
}

TNodeI MultimodalGraphImplB::BegNI() const {
  return TNodeI(NodeToModeMapping.BegI(), &Graphs, &NodeToModeMapping);
}

TNodeI MultimodalGraphImplB::EndNI() const {
  return TNodeI(NodeToModeMapping.EndI(), &Graphs, &NodeToModeMapping);
}

TNodeI MultimodalGraphImplB::GetNI(const int& NId) const {
  return TNodeI(NodeToModeMapping.GetI(NId), &Graphs, &NodeToModeMapping);
}

TEdgeI MultimodalGraphImplB::GetEI(const int& SrcNId, const int& DstNId) const {
  TNodeI CurNode = TNodeI(NodeToModeMapping.GetI(SrcNId), &Graphs, &NodeToModeMapping);
  TIntV AdjacentModes = TIntV();
  CurNode.GetAdjacentModes(AdjacentModes);

  TPair<TInt,TInt> ModeIdsKey = GetModeIdsKey(NodeToModeMapping.GetDat(SrcNId), NodeToModeMapping.GetDat(DstNId));

  int CurAdjacentMode = 0;
  for (int i=0; i<AdjacentModes.Len();i++) {
    if (AdjacentModes.GetDat(i) == DstModeId) {
      CurAdjacentMode=i;
      break;
    }
  }

  return TEdgeI(CurNode, EndNI(), Graphs.GetDat(ModeIdsKey).GetEI(SrcNId, DstNId).GetCurEdge(), AdjacentModes, CurAdjacentMode);
}

void TMultimodalGraphImplB::GetNIdV(TIntV& NIdV) const {
  NIdV.Gen(GetNodes(), 0);
  for (int N=NodeToModeMapping; NodeToModeMapping.FNextKeyId(N); ) {
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

void TMultimodalGraphImplB::Defrag(const bool& OnlyNodeLinks=false) {
  for (TGraphs::TIter it = Graphs.BegI(); it < Graphs.EndI(); it++) {
    it.GetDat().Defrag(OnlyNodeLinks);
  }
}

bool TMultimodalGraphImplB::IsOk(const bool& ThrowExcept=true) const {
  for (TGraphs::TIter it = Graphs.BegI(); it < Graphs.EndI(); it++) {
    if (!it.GetDat().IsOk(ThrowExcept)) {
      return false;
    }
  }
  return true;
}

PMultimodalGraphImplB TMultimodalGraphImplB::GetSmallGraph() {
  PMultimodalGraphImplB G = TMultimodalGraphImplB::New();
  for (int i = 0; i < 5; i++) { G->AddNode(i, 0); }
  for (int i = 0; i < 5; i++) { G->AddNode(i, 1); }
  G->AddEdge(0,1); G->AddEdge(1,2); G->AddEdge(0,2);
  G->AddEdge(1,3); G->AddEdge(3,4); G->AddEdge(2,3);
  G->AddEdge(5,6); G->AddEdge(5,7); G->AddEdge(6,7);
  G->AddEdge(6,9); G->AddEdge(6,8); G->AddEdge(8,9);
  return G;
}

