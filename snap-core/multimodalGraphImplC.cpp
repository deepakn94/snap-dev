/////////////////////////////////////////////////
// Multi-modal Graph, Impl C

int TMultimodalGraphImplC::GetNodes() const {
  int NumNodes = 0;
  for (TNodeH::TIter Node=NodeH.BegI(); Node<NodeH.EndI(); Node++) {
    NumNodes += Node.GetDat().Len();
  }
  return NumNodes;
}

int TMultimodalGraphImplC::AddNode(TPair<TInt,TInt> NId) {
  int LocalNId = NId.GetVal2();
  int ModeId = NId.GetVal1();
  if (NId.GetVal2() == -1) {
    LocalNId = MxNId;  MxNId++;
  } else {
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId (%d,%d) already exists", NId.GetVal1(), NId.GetVal2()));
    MxNId = TMath::Mx(LocalNId+1, MxNId());
  }
  if (!NodeH.IsKey(ModeId)) {
    NodeH.AddDat(ModeId);
  }
  NodeH[ModeId].AddDat(LocalNId,TNode(LocalNId));
  return NId;
}

void TMultimodalGraphImplC::DelNode(TPair<TInt,TInt> NId) {
  int LocalNId = NId.GetVal2();
  int ModeId = NId.GetVal1();
  IAssertR(IsNode(NId), TStr::Fmt("NodeId (%d,%d) doesn't exist", NId.GetVal1(), NId.GetVal2()));

  TNode& Node = NodeH[ModeId][LocalNId];
  for (int e = 0; e < Node.GetOutDeg(); e++) {
    const TPair<TInt,TInt> nbr = Node.GetOutNId(e);
    if (nbr == NId) { continue; }
    TNode& N = NodeH[nbr.GetVal1()][nbr.GetVal2()];
    const int n = N.InNIdV.SearchBin(NId);
    if (n!= -1) { N.InNIdV.Del(n); NEdges--; }
  }
  for (int e = 0; e < Node.GetInDeg(); e++) {
    const TPair<TInt,TInt> nbr = Node.GetInNId(e);
    if (nbr == NId) { continue; }
    TNode& N = NodeH[nbr.GetVal1()][nbr.GetVal2()];
    const int n = N.OutNIdV.SearchBin(NId);
    if (n!= -1) { N.OutNIdV.Del(n); NEdges--; }
  }
}

bool TMultimodalGraphImplC::IsNode(TPair<TInt,TInt> NId) {
  int ModeId = NId.GetVal1();
  int LocalNId = NId.GetVal2();
  if (!NodeH.IsKey(ModeId)) {
    return false;
  }
  if (!NodeH[ModeId].IsKey(LocalNId)) {
    return false;
  }
  return true;
}

TNodeI TMultimodalGraphImplC::GetNI(const TPair<TInt,TInt>& NId) const {
  IAssertR(IsNode(NId), TStr::Fmt("NodeId (%d,%d) doesn't exist", NId.GetVal1(), NId.GetVal2()));
  int ModeId = NId.GetVal1();
  int LocalNId = NId.GetVal2();

  return TNodeI(NodeH.GetI(ModeId), NodeH.EndI(), NodeH.GetDat(ModeId).GetI(NodeId), NodeH.GetDat(ModeId).EndI());
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

  NodeH[SrcModeId][SrcLocalNId].OutNIdV.AddSorted(DstNId);
  NodeH[DstModeId][DstLocalNId].InNIdV.AddSorted(SrcNId);
  return -1;
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

  NodeH[SrcModeId][SrcLocalNId].OutNIdV.DelIfIn(DstNId);
  NodeH[DstModeId][DstLocalNId].InNIdV.DelIfIn(SrcNId);
}

bool TMultimodalGraphImplC::IsEdge(const TPair<TInt,TInt>& SrcNId, const TPair<TInt,TInt>& DstNId) {
  if (! IsNode(SrcNId) || !IsNode(DstNId)) { return false; }
  int SrcModeId = SrcNId.GetVal1();
  int SrcLocalNId = SrcNId.GetVal2();
  return NodeH[SrcModeId][SrcLocalNId].IsOutNId(DstNId);
}

TEdgeI TMultimodalGraphImplC::GetEI(const TPair<TInt,TInt>& SrcNId, const TPair<TInt,TInt>& DstNId) {
  const TNodeI SrcNI = GetNI(SrcNId);
  const int NodeN = SrcNI.NodeNHI.GetDat().OutNIdV.SearchBin(DstNId);
  if (NodeN == -1) { return EndEI(); }
  return TEdgeI(SrcNI, EndNI(), NodeN);
}

PMultimodalGraphImplA TMultimodalGraphImplC::GetSmallGraph() {
  PMultimodalGraphImplA G = TMultimodalGraphImplC::New();
  for (int i = 0; i < 5; i++) { G->AddNode(TPair<TInt,TInt>(0,i)); }
  for (int i = 0; i < 5; i++) { G->AddNode(TPair<TInt,TInt>(1,i)); }
  G->AddEdge(TPair<TInt,TInt>(0,0),TPair<TInt,TInt>(0,1)); 
  G->AddEdge(TPair<TInt,TInt>(0,1),TPair<TInt,TInt>(0,2));
  G->AddEdge(TPair<TInt,TInt>(0,0),TPair<TInt,TInt>(0,2));
  G->AddEdge(TPair<TInt,TInt>(0,1),TPair<TInt,TInt>(0,3));
  G->AddEdge(TPair<TInt,TInt>(0,3),TPair<TInt,TInt>(0,4));
  G->AddEdge(TPair<TInt,TInt>(0,2),TPair<TInt,TInt>(0,3));
  G->AddEdge(TPair<TInt,TInt>(1,5),TPair<TInt,TInt>(1,6));
  G->AddEdge(TPair<TInt,TInt>(1,5),TPair<TInt,TInt>(1,7));
  G->AddEdge(TPair<TInt,TInt>(1,6),TPair<TInt,TInt>(1,7));
  G->AddEdge(TPair<TInt,TInt>(1,6),TPair<TInt,TInt>(1,9));
  G->AddEdge(TPair<TInt,TInt>(1,6),TPair<TInt,TInt>(1,8));
  G->AddEdge(TPair<TInt,TInt>(1,8),TPair<TInt,TInt>(1,9));
  return G;
}

