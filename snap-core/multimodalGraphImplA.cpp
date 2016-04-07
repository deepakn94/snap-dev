/////////////////////////////////////////////////
// Multi-modal Graph, Impl A

int TMultimodalGraphImplA::AddNode(int NId, int ModeId) {
  if (NId == -1) {
    NId = MxNId;  MxNId++;
  } else {
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
    MxNId = TMath::Mx(NId+1, MxNId());
  }
  Network.AddNode(NId, ModeId);
  return NId;
}

int TMultimodalGraphImplA::AddEdge(const int& SrcNId, const int& DstNId) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
  if (IsEdge(SrcNId, DstNId)) {
    return -2; // Edge already exists
  }
  NEdges++;
  return Network.AddEdge(SrcNId, DstNId);
}

void TMultimodalGraphImplA::DelEdge(const int& SrcNId, const int& DstNId) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
  if (!IsEdge(SrcNId, DstNId)) {
    return; // Edge doesn't exist
  }
  NEdges--;
  Network.DelEdge(SrcNId, DstNId);
}

PMultimodalGraphImplA TMultimodalGraphImplA::GetSmallGraph() {
  PMultimodalGraphImplA G = TMultimodalGraphImplA::New();
  for (int i = 0; i < 5; i++) { G->AddNode(i, 0); }
  for (int i = 0; i < 5; i++) { G->AddNode(i, 1); }
  G->AddEdge(0,1); G->AddEdge(1,2); G->AddEdge(0,2);
  G->AddEdge(1,3); G->AddEdge(3,4); G->AddEdge(2,3);
  G->AddEdge(5,6); G->AddEdge(5,7); G->AddEdge(6,7);
  G->AddEdge(6,9); G->AddEdge(6,8); G->AddEdge(8,9);
  return G;
}

