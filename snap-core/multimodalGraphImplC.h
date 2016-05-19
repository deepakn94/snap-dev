//#//////////////////////////////////////////////
/// Multimodal graph: Implementation C
/// Single graph with all nodes [with global ids], each node labelled
/// with appropriate attribute

class TMultimodalGraphImplC;

/// Pointer to a multimodal graph
typedef TPt<TMultimodalGraphImplC> PMultimodalGraphImplC;

//#//////////////////////////////////////////////
/// Multimodal graph. ##TMultimodalGraphImplC::Class
class TMultimodalGraphImplC {
public:
  TCRef CRef;
  class TNode {
  private:
    TInt Id;
    TVec< TPair<TInt,TInt> > InNIdV, OutNIdV;
  public:
    TNode() : Id(-1), InNIdV(), OutNIdV() { }
    TNode(const int& NId) : Id(NId), InNIdV(), OutNIdV() { }
    TNode(const TNode& Node) : Id(Node.Id), InNIdV(Node.InNIdV), OutNIdV(Node.OutNIdV) { }
    TNode(TSIn& SIn) : Id(SIn), InNIdV(SIn), OutNIdV(SIn) { }
    void Save(TSOut& SOut) const { Id.Save(SOut); InNIdV.Save(SOut); OutNIdV.Save(SOut); }
    int GetId() const { return Id; }
    int GetDeg() const { return GetInDeg() + GetOutDeg(); }
    int GetInDeg() const { return InNIdV.Len(); }
    int GetOutDeg() const { return OutNIdV.Len(); }
    int GetStartingIdx(int ModeId) const {
      int StartingIdx; OutNIdV.SearchBin(TPair<TInt,TInt>(ModeId,-1), StartingIdx);
      return StartingIdx;
    }
    int GetEndingIdx(int ModeId) const {
      int EndingIdx; OutNIdV.SearchBin(TPair<TInt,TInt>(ModeId+1,-1), EndingIdx);
      return EndingIdx;
    }
    TPair<TInt,TInt> GetInNId(const int& NodeN) const { return InNIdV[NodeN]; }
    TPair<TInt,TInt> GetOutNId(const int& NodeN) const { return OutNIdV[NodeN]; }
    TPair<TInt,TInt> GetNbrNId(const int& NodeN) const { return NodeN<GetOutDeg()?GetOutNId(NodeN):GetInNId(NodeN-GetOutDeg()); }
    bool IsInNId(const TPair<TInt,TInt>& NId) const { return InNIdV.SearchBin(NId) != -1; }
    bool IsOutNId(const TPair<TInt,TInt>& NId) const { return OutNIdV.SearchBin(NId) != -1; }
    bool IsNbrNId(const TPair<TInt,TInt>& NId) const { return IsOutNId(NId) || IsInNId(NId); }
    void PackOutNIdV() { OutNIdV.Pack(); }
    void PackNIdV() { InNIdV.Pack(); }
    void SortNIdV() { InNIdV.Sort(); OutNIdV.Sort();}
    friend class TMultimodalGraphImplC;
  };
  /// Node iterator. Only forward iteration (operator++) is supported.
  class TNodeI {
  private:
    typedef THash< TInt, THash<TInt, TNode> >::TIter TModeHashIter;
    typedef THash<TInt, TNode>::TIter TNodeHashIter;
    TModeHashIter NodeMHI, EndNodeMHI;
    TNodeHashIter NodeNHI, EndNodeNHI;
  public:
    TNodeI() : NodeMHI(), NodeNHI() { }
    TNodeI(const TModeHashIter& NodeMHIter,const TModeHashIter& EndNodeMHIter,const TNodeHashIter& NodeNHIter) :
      NodeMHI(NodeMHIter), EndNodeMHI(EndNodeMHIter), NodeNHI(NodeNHIter), EndNodeNHI(NodeMHI.GetDat().EndI()) { }
    TNodeI(const TModeHashIter& NodeMHIter, const TModeHashIter& EndNodeMHIter) :
      NodeMHI(NodeMHIter), EndNodeMHI(EndNodeMHIter), NodeNHI(NodeMHI.GetDat().BegI()), EndNodeNHI(NodeMHI.GetDat().EndI()) { }
    TNodeI(const TNodeI& NodeI) : NodeMHI(NodeI.NodeMHI), EndNodeMHI(NodeI.EndNodeMHI), NodeNHI(NodeI.NodeNHI), EndNodeNHI(NodeI.EndNodeNHI) { }
    TNodeI& operator = (const TNodeI& NodeI) { NodeMHI = NodeI.NodeMHI; EndNodeMHI = NodeI.EndNodeMHI; NodeNHI = NodeI.NodeNHI; EndNodeNHI = NodeI.EndNodeNHI; return *this; }
    /// Increment iterator.
    TNodeI& operator++ (int) {
      NodeNHI++;
      if (!(NodeNHI < EndNodeNHI)) {
        NodeMHI++;
        if (NodeMHI < EndNodeMHI) {
          NodeNHI = NodeMHI.GetDat().BegI();
          EndNodeNHI = NodeMHI.GetDat().EndI();
        }
      }
      return *this;
    }

    bool operator < (const TNodeI& NodeI) const { return (NodeMHI < NodeI.NodeMHI) || (NodeMHI == NodeI.NodeMHI && NodeNHI < NodeI.NodeNHI); }
    bool operator == (const TNodeI& NodeI) const { return (NodeMHI == NodeI.NodeMHI) && (NodeNHI == NodeI.NodeNHI); }
    /// Returns ID of the current node.
    TPair<TInt,TInt> GetId() const { return TPair<TInt,TInt>(NodeMHI.GetKey(),NodeNHI.GetDat().GetId()); }
    /// Returns degree of the current node, the sum of in-degree and out-degree.
    int GetDeg() const { return NodeNHI.GetDat().GetDeg(); }
    /// Returns in-degree of the current node.
    int GetInDeg() const { return NodeNHI.GetDat().GetInDeg(); }
    /// Returns out-degree of the current node.
    int GetOutDeg() const { return NodeNHI.GetDat().GetOutDeg(); }
    /// Sorts the adjacency lists of the current node.
    void SortNIdV() { NodeNHI.GetDat().SortNIdV(); }
    /// Returns ID of NodeN-th in-node (the node pointing to the current node). ##TMultimodalGraphImplC::TNodeI::GetInNId
    TPair<TInt,TInt> GetInNId(const int& NodeN) const { return NodeNHI.GetDat().GetInNId(NodeN); }
    /// Returns ID of NodeN-th out-node (the node the current node points to). ##TMultimodalGraphImplC::TNodeI::GetOutNId
    TPair<TInt,TInt> GetOutNId(const int& NodeN) const { return NodeNHI.GetDat().GetOutNId(NodeN); }
    /// Returns ID of NodeN-th neighboring node. ##TMultimodalGraphImplC::TNodeI::GetNbrNId
    TPair<TInt,TInt> GetNbrNId(const int& NodeN) const { return NodeNHI.GetDat().GetNbrNId(NodeN); }
    /// Returns ModeId of current node
    int GetModeId() const { return NodeMHI.GetKey(); }
    /// Tests whether node with ID NId points to the current node.
    bool IsInNId(const TPair<TInt,TInt>& NId) const { return NodeNHI.GetDat().IsInNId(NId); }
    /// Tests whether the current node points to node with ID NId.
    bool IsOutNId(const TPair<TInt,TInt>& NId) const { return NodeNHI.GetDat().IsOutNId(NId); }
    /// Tests whether node with ID NId is a neighbor of the current node.
    bool IsNbrNId(const TPair<TInt,TInt>& NId) const { return IsOutNId(NId) || IsInNId(NId); }
    friend class TMultimodalGraphImplC;
  };
  /// Edge iterator. Only forward iteration (operator++) is supported.
  class TEdgeI {
  private:
    TNodeI CurNode, EndNode;
    int CurEdge;
  public:
    TEdgeI() : CurNode(), EndNode(), CurEdge(0) { }
    TEdgeI(const TNodeI& NodeI, const TNodeI& EndNodeI, const int& EdgeN=0) : CurNode(NodeI), EndNode(EndNodeI), CurEdge(EdgeN) { }
    TEdgeI(const TEdgeI& EdgeI) : CurNode(EdgeI.CurNode), EndNode(EdgeI.EndNode), CurEdge(EdgeI.CurEdge) { }
    TEdgeI& operator = (const TEdgeI& EdgeI) { if (this!=&EdgeI) { CurNode=EdgeI.CurNode; EndNode=EdgeI.EndNode; CurEdge=EdgeI.CurEdge; }  return *this; }
    /// Increment iterator.
    TEdgeI& operator++ (int) { CurEdge++; if (CurEdge >= CurNode.GetOutDeg()) { CurEdge=0; CurNode++;
      while (CurNode < EndNode && CurNode.GetOutDeg()==0) { CurNode++; } }  return *this; }
    bool operator < (const TEdgeI& EdgeI) const { return CurNode<EdgeI.CurNode || (CurNode==EdgeI.CurNode && CurEdge<EdgeI.CurEdge); }
    bool operator == (const TEdgeI& EdgeI) const { return CurNode == EdgeI.CurNode && CurEdge == EdgeI.CurEdge; }
    /// Returns edge ID. Always returns -1 since only edges in multigraphs have explicit IDs.
    int GetId() const { return -1; }
    /// Returns the source node of the edge.
    TPair<TInt,TInt> GetSrcNId() const { return CurNode.GetId(); }
    /// Returns the destination node of the edge.
    TPair<TInt,TInt> GetDstNId() const { return CurNode.GetOutNId(CurEdge); }
    /// Returns NodeN of current edge.
    int GetCurEdge() const { return CurEdge; }
    friend class TMultimodalGraphImplC;
  };
private:
  TInt MxNId, NEdges;
  // Encapsulate a network under the hood
  typedef THash< TInt,THash<TInt,TNode> > TNodeH;
  TNodeH NodeH;
public:
  TMultimodalGraphImplC() : CRef(), MxNId(0), NEdges(0), NodeH() { }
  /// Constructor that reserves enough memory for a graph of Nodes nodes and Edges edges.
  explicit TMultimodalGraphImplC(const int& Nodes, const int& Edges) : CRef(), MxNId(0), NEdges(0) { Reserve(Nodes, Edges); }
  TMultimodalGraphImplC(const TMultimodalGraphImplC& Graph) : MxNId(Graph.MxNId), NEdges(Graph.NEdges), NodeH(Graph.NodeH) { }
  /// Constructor that loads the graph from a (binary) stream SIn.
  TMultimodalGraphImplC(TSIn& SIn) : MxNId(SIn), NEdges(SIn), NodeH(NodeH) { }
  virtual ~TMultimodalGraphImplC() { }
  /// Saves the graph to a (binary) stream SOut.
  void Save(TSOut& SOut) const { MxNId.Save(SOut); NEdges.Save(SOut); NodeH.Save(SOut); }
  /// Static constructor that returns a pointer to the graph. Call: PMultimodalGraphImplC Graph = TMultimodalGraphImplCGraph::New().
  static PMultimodalGraphImplC New() { return new TMultimodalGraphImplC(); }
  /// Static constructor that returns a pointer to the graph and reserves enough memory for Nodes nodes and Edges edges. ##TMultimodalGraphImplC::New
  static PMultimodalGraphImplC New(const int& Nodes, const int& Edges) { return new TMultimodalGraphImplC(Nodes, Edges); }
  /// Static constructor that loads the graph from a stream SIn and returns a pointer to it.
  static PMultimodalGraphImplC Load(TSIn& SIn) { return PMultimodalGraphImplC(new TMultimodalGraphImplC(SIn)); }
  /// Allows for run-time checking the type of the graph (see the TGraphFlag for flags).
  // TODO: Implement this
  bool HasFlag(const TGraphFlag& Flag) const;
  TMultimodalGraphImplC& operator = (const TMultimodalGraphImplC& Graph) {
    if (this!=&Graph) { MxNId=Graph.MxNId; NEdges=Graph.NEdges; NodeH=Graph.NodeH; } return *this; }
  
  /// Returns the number of nodes in the graph.
  int GetNodes() const;
  /// Adds a node of ID NId and ModeId MId to the multimodal graph.
  TPair<TInt,TInt> AddNode(int ModeId);
  void AddNode(const TPair<TInt,TInt>& NodeId);
  /// Deletes node of ID NId from the graph. ##TUNGraph::DelNode
  void DelNode(TPair<TInt,TInt> NId);
  /// Tests whether ID NId is a node.
  bool IsNode(TPair<TInt,TInt> NId);
  /// Returns an iterator referring to the first node in the graph.
  TNodeI BegNI() const { return TNodeI(NodeH.BegI(), NodeH.EndI()); }
  /// Returns an iterator referring to the past-the-end node in the graph.
  TNodeI EndNI() const { return TNodeI(NodeH.EndI(), NodeH.EndI()); }
  /// Returns an iterator referring to the node of ID NId in the graph.
  TNodeI GetNI(const TPair<TInt,TInt>& NId) const;
  /// Returns an ID that is larger than any node ID in the graph.
  int GetMxNId() const { return MxNId; }
  /// Populates vector of Node Ids contained in the specific mode.
  void GetNodeIdsInMode(const int ModeId, TVec< TPair<TInt,TInt> >& NodeIds) const;

  /// Returns the number of edges in the graph.
  int GetEdges() const { return NEdges; }
  /// Adds an edge between node IDs SrcNId and DstNId to the graph. ##TUNGraph::AddEdge
  int AddEdge(const TPair<TInt,TInt>& SrcNId, const TPair<TInt,TInt>& DstNId);
  void AddEdgeBatch(const TPair<TInt,TInt>& SrcNId, const TVec<TPair<TInt,TInt> >& DstNIds);
  /// Deletes an edge between node IDs SrcNId and DstNId from the graph. ##TUNGraph::DelEdge
  void DelEdge(const TPair<TInt,TInt>& SrcNId, const TPair<TInt,TInt>& DstNId);
  /// Tests whether an edge between node IDs SrcNId and DstNId exists in the graph.
  bool IsEdge(const TPair<TInt,TInt>& SrcNId, const TPair<TInt,TInt>& DstNId);
  /// Returns an iterator referring to the first edge in the graph.
  TEdgeI BegEI() const { TNodeI NI = BegNI(); TEdgeI EI(NI, EndNI(), 0); while (NI < EndNI() && (NI.GetOutDeg()==0)) { EI++; } return EI; }
  /// Returns an iterator referring to the past-the-end edge in the graph.
  TEdgeI EndEI() const { return TEdgeI(EndNI(), EndNI()); }
  /// Returns an iterator referring to edge (SrcNId, DstNId) in the graph. ##TUNGraph::GetEI
  TEdgeI GetEI(const TPair<TInt,TInt>& SrcNId, const TPair<TInt,TInt>& DstNId) const;

  /// Transformation methods from multi-modal graph to regular, directed graph.
  TIntNNet GetSubGraph(const TIntV ModeIds) const;
  int GetSubGraphMocked(const TIntV ModeIds) const;
  int BFSTraversalOneHop(const TVec< TPair<TInt,TInt> >& StartingVertices) const;

  /// Returns an ID of a random node in the graph.
  int GetRndNId(TRnd& Rnd=TInt::Rnd);
  /// Returns an interator referring to a random node in the graph.
  TNodeI GetRndNI(TRnd& Rnd=TInt::Rnd);
  /// Gets a vector IDs of all nodes in the graph.
  void GetNIdV(TIntV& NIdV) const;

  /// Tests whether the graph is empty (has zero nodes).
  bool Empty() const { return GetNodes()==0; }
  /// Deletes all nodes and edges from the graph.
  void Clr();
  /// Reserves memory for a graph of Nodes nodes and Edges edges.
  void Reserve(const int& Nodes, const int& Edges);
  void ReserveOutNIdV(const TPair<TInt,TInt>& NId, const int& Size) {
    NodeH.GetDat(NId.GetVal1()).GetDat(NId.GetVal2()).OutNIdV.Reserve(Size);
  }
  /// Defragments the graph. ##TUNGraph::Defrag
  void Defrag(const bool& OnlyNodeLinks=false);
  /// Checks the graph data structure for internal consistency. ##TUNGraph::IsOk
  bool IsOk(const bool& ThrowExcept=true) const;
  /// Returns a small graph on 10 nodes and 10 edges with 2 modes. ##TUNGraph::GetSmallGraph
  static PMultimodalGraphImplC GetSmallGraph();
};
