//#//////////////////////////////////////////////
/// Multimodal graph: Implementation B
/// k(k+1)/2 graphs between the same set of vertices

class TMultimodalGraphImplB;

/// Pointer to a multimodal graph
typedef TPt<TMultimodalGraphImplB> PMultimodalGraphImplB;
typedef THash<TPair<TInt,TInt>, TNGraph> TGraphs;
typedef TNGraph::TNode TNode;

//#//////////////////////////////////////////////
/// Multimodal graph. ##TMultimodalGraphImplB::Class
class TMultimodalGraphImplB {
private:
  static TPair<TInt,TInt> GetModeIdsKey(const int &SrcModeId, const int &DstModeId) {
    int MinModeId = TMath::Mn(SrcModeId, DstModeId);
    int MaxModeId = TMath::Mx(SrcModeId, DstModeId);
    TPair<TInt,TInt> ModeIdsKey = TPair<TInt,TInt>(MinModeId, MaxModeId);

    return ModeIdsKey;
  }
public:
  TCRef CRef;
  /// Node iterator. Only forward iteration (operator++) is supported.
  class TNodeI {
  private:
    typedef THash<TInt,TInt>::TIter THashIter;
    THashIter NodeHI;
    TGraphs *Graphs;
    THash<TInt,TInt> *NodeToModeMapping;
  public:
    TNodeI() : NodeHI() { }
    TNodeI(const THashIter& NodeHIter, const TGraphs *Graphs, const THash<TInt,TInt> *NodeToModeMapping) :
      NodeHI(NodeHIter),
      Graphs((TGraphs *) Graphs),
      NodeToModeMapping((THash<TInt,TInt> *) NodeToModeMapping) { }
    TNodeI(const TNodeI& NodeI) : NodeHI(NodeI.NodeHI), Graphs(NodeI.Graphs), NodeToModeMapping(NodeI.NodeToModeMapping) { }
    TNodeI& operator = (const TNodeI& NodeI) { NodeHI = NodeI.NodeHI; Graphs = NodeI.Graphs; NodeToModeMapping = NodeI.NodeToModeMapping; return *this; }
    /// Increment iterator.
    TNodeI& operator++ (int) { NodeHI++; return *this; }
    /// Decrement iterator.
    TNodeI& operator-- (int) { NodeHI--; return *this; }

    bool operator < (const TNodeI& NodeI) const { return NodeHI < NodeI.NodeHI; }
    bool operator == (const TNodeI& NodeI) const { return NodeHI == NodeI.NodeHI; }
    /// Returns ID of the current node.
    int GetId() const { return NodeHI.GetKey(); }
    /// Returns degree of the current node, the sum of in-degree and out-degree.
    int GetDeg() const { return GetInDeg() + GetOutDeg(); }
    /// Returns in-degree of the current node.
    int GetInDeg() const;
    int GetInDeg(const int &ModeId) const;
    /// Returns out-degree of the current node.
    int GetOutDeg() const;
    int GetOutDeg(const int &ModeId) const;
    /// Sorts the adjacency lists of the current node.
    void SortNIdV();
    /// Returns ID of NodeN-th in-node (the node pointing to the current node) from a particular mode. ##TNGraph::TNodeI::GetInNId
    int GetInNId(const int& NodeN, const int& ModeId) const {
      return Graphs->GetDat(GetModeIdsKey(NodeToModeMapping->GetDat(GetId()), ModeId)).GetNI(GetId()).GetInNId(NodeN);
    }
    /// Returns ID of NodeN-th out-node (the node the current node points to) to a particular mode. ##TNGraph::TNodeI::GetOutNId
    int GetOutNId(const int& NodeN, const int& ModeId) const {
      return Graphs->GetDat(GetModeIdsKey(NodeToModeMapping->GetDat(GetId()), ModeId)).GetNI(GetId()).GetOutNId(NodeN);
    }
    /// Returns ID of NodeN-th neighboring node in a given mode. ##TNGraph::TNodeI::GetNbrNId
    int GetNbrNId(const int& NodeN, const int& ModeId) const {
      return Graphs->GetDat(GetModeIdsKey(NodeToModeMapping->GetDat(GetId()), ModeId)).GetNI(GetId()).GetNbrNId(NodeN);
    }
    /// Tests whether node with ID NId points to the current node.
    bool IsInNId(const int& NId) const {
      return Graphs->GetDat(GetModeIdsKey(NodeToModeMapping->GetDat(GetId()), NodeToModeMapping->GetDat(NId))).GetNI(GetId()).IsInNId(NId);
    }
    /// Tests whether the current node points to node with ID NId.
    bool IsOutNId(const int& NId) const {
      return Graphs->GetDat(GetModeIdsKey(NodeToModeMapping->GetDat(GetId()), NodeToModeMapping->GetDat(NId))).GetNI(GetId()).IsOutNId(NId);
    }
    /// Tests whether node with ID NId is a neighbor of the current node.
    bool IsNbrNId(const int& NId) const {
      return IsOutNId(NId) || IsInNId(NId);
    }
    /// Get all adjacent modes.
    void GetAdjacentModes(TIntV &AdjacentModes) const;
    friend class TMultimodalGraphImplB;
  };
  /// Edge iterator. Only forward iteration (operator++) is supported.
  class TEdgeI {
  private:
    TNodeI CurNode, EndNode;
    int CurEdge;
    TIntV AdjacentModes;
    int CurAdjacentMode;
  public:
    TEdgeI() : CurNode(), EndNode(), CurEdge(0), AdjacentModes(), CurAdjacentMode(0) { }
    TEdgeI(const TNodeI& NodeI, const TNodeI& EndNodeI, const int& EdgeN=0) :
      CurNode(NodeI), EndNode(EndNodeI), CurEdge(EdgeN), AdjacentModes(), CurAdjacentMode(0) { 
      CurNode.GetAdjacentModes(AdjacentModes);
    }
    TEdgeI(const TNodeI& NodeI, const TNodeI& EndNodeI, const int& EdgeN, const TIntV& AdjacentModes, const int& CurAdjacentMode) :
      CurNode(NodeI), EndNode(EndNodeI), CurEdge(EdgeN), AdjacentModes(AdjacentModes),
      CurAdjacentMode(CurAdjacentMode)
    {}
    TEdgeI(const TEdgeI& EdgeI) :
      CurNode(EdgeI.CurNode), EndNode(EdgeI.EndNode), CurEdge(EdgeI.CurEdge),
      AdjacentModes(EdgeI.AdjacentModes), CurAdjacentMode(EdgeI.CurAdjacentMode) { }
    TEdgeI& operator = (const TEdgeI& EdgeI) { if (this!=&EdgeI) { CurNode=EdgeI.CurNode; EndNode=EdgeI.EndNode; CurEdge=EdgeI.CurEdge; AdjacentModes=EdgeI.AdjacentModes; CurAdjacentMode=EdgeI.CurAdjacentMode; }  return *this; }

    /// Increment iterator.
    TEdgeI& operator++ (int) {
      CurEdge++;
      if (CurEdge >= CurNode.GetOutDeg()) {
        CurAdjacentMode++;
        CurEdge=0;
        if (CurAdjacentMode >= AdjacentModes.Len()) {
          CurEdge=0; CurNode++; CurAdjacentMode=0; AdjacentModes.Clr();
          while (CurNode < EndNode && CurNode.GetOutDeg()==0) {
            CurNode++;
          }
          CurNode.GetAdjacentModes(AdjacentModes);
        }
      }
      return *this;
    }
    bool operator < (const TEdgeI& EdgeI) const { return CurNode<EdgeI.CurNode || (CurNode==EdgeI.CurNode && CurEdge<EdgeI.CurEdge && CurAdjacentMode==EdgeI.CurAdjacentMode) || (CurNode==EdgeI.CurNode && CurAdjacentMode<EdgeI.CurAdjacentMode); }
    bool operator == (const TEdgeI& EdgeI) const { return CurNode == EdgeI.CurNode && CurEdge == EdgeI.CurEdge && CurAdjacentMode == EdgeI.CurAdjacentMode; }

    /// Returns edge ID. Always returns -1 since only edges in multigraphs have explicit IDs.
    int GetId() const { return -1; }
    /// Returns the source node of the edge.
    int GetSrcNId() const { return CurNode.GetId(); }
    /// Returns the destination node of the edge.
    int GetDstNId() const { return CurNode.GetOutNId(CurEdge, AdjacentModes.GetVal(CurAdjacentMode)); }
    friend class TMultimodalGraphImplB;
  };
private:
  TInt MxNId, NEdges;
  // Collection of K(K+1)/2 graphs and mapping from NId to ModeId
  THash<TInt,TInt> NodeToModeMapping;
  THash<TPair<TInt,TInt>,TNGraph> Graphs;
public:
  TMultimodalGraphImplB() : CRef(), MxNId(0), NEdges(0), NodeToModeMapping(), Graphs() { }
  /// Constructor that reserves enough memory for a graph of Nodes nodes and Edges edges.
  explicit TMultimodalGraphImplB(const int& Nodes, const int& Edges) : CRef(), MxNId(0), NEdges(0) { Reserve(Nodes, Edges); }
  TMultimodalGraphImplB(const TMultimodalGraphImplB& Graph) : MxNId(Graph.MxNId), NEdges(Graph.NEdges), NodeToModeMapping(Graph.NodeToModeMapping), Graphs(Graph.Graphs) { }
  /// Constructor that loads the graph from a (binary) stream SIn.
  TMultimodalGraphImplB(TSIn& SIn) : MxNId(SIn), NEdges(SIn), NodeToModeMapping(SIn), Graphs(SIn) { }
  virtual ~TMultimodalGraphImplB() { }
  /// Saves the graph to a (binary) stream SOut.
  void Save(TSOut& SOut) const { MxNId.Save(SOut); NEdges.Save(SOut); NodeToModeMapping.Save(SOut); Graphs.Save(SOut); }
  /// Static constructor that returns a pointer to the graph. Call: PMultimodalGraphImplB Graph = TMultimodalGraphImplBGraph::New().
  static PMultimodalGraphImplB New() { return new TMultimodalGraphImplB(); }
  /// Static constructor that returns a pointer to the graph and reserves enough memory for Nodes nodes and Edges edges. ##TMultimodalGraphImplB::New
  static PMultimodalGraphImplB New(const int& Nodes, const int& Edges) { return new TMultimodalGraphImplB(Nodes, Edges); }
  /// Static constructor that loads the graph from a stream SIn and returns a pointer to it.
  static PMultimodalGraphImplB Load(TSIn& SIn) { return PMultimodalGraphImplB(new TMultimodalGraphImplB(SIn)); }
  /// Allows for run-time checking the type of the graph (see the TGraphFlag for flags).
  /// TODO: Implement this
  bool HasFlag(const TGraphFlag& Flag) const;
  TMultimodalGraphImplB& operator = (const TMultimodalGraphImplB& Graph) {
    if (this!=&Graph) { MxNId=Graph.MxNId; NEdges=Graph.NEdges; NodeToModeMapping=Graph.NodeToModeMapping; Graphs=Graph.Graphs; } return *this; }
  
  /// Returns the number of nodes in the graph.
  int GetNodes() const { return NodeToModeMapping.Len(); }
  /// Adds a node of ID NId and ModeId MId to the multimodal graph.
  TPair<TInt,TInt> AddNode(int ModeId);
  void AddNode(const TPair<TInt,TInt>& NodeId);
  /// Deletes node of ID NId from the graph. ##TMultimodalGraphImplB::DelNode
  void DelNode(const TPair<TInt,TInt>& NId);
  /// Tests whether ID NId is a node.
  bool IsNode(const TPair<TInt,TInt>& NId) const { return NodeToModeMapping.IsKey(NId.GetVal2()); }
  /// Returns an iterator referring to the first node in the graph.
  TNodeI BegNI() const;
  /// Returns an iterator referring to the past-the-end node in the graph.
  TNodeI EndNI() const;
  /// Returns an iterator referring to the node of ID NId in the graph.
  TNodeI GetNI(const TPair<TInt,TInt>& NId) const;
  /// Returns an ID that is larger than any node ID in the graph.
  int GetMxNId() const { return MxNId; }
  /// Populates vector of Node Ids contained in the specific mode.
  void GetNodeIdsInMode(const int ModeId, TVec< TPair<TInt,TInt> >& NodeIds) const;

  /// Returns the number of edges in the graph.
  int GetEdges() const { return NEdges; }
  /// Adds an edge between node IDs SrcNId and DstNId to the graph. ##TMultimodalGraphImplB::AddEdge
  int AddEdge(const TPair<TInt,TInt>& SrcNId, const TPair<TInt,TInt>& DstNId);
  void AddEdgeBatch(const TPair<TInt,TInt>& SrcNId, const TVec<TPair<TInt,TInt> >&DstNIds);
  /// Deletes an edge between node IDs SrcNId and DstNId from the graph. ##TMultimodalGraphImplB::DelEdge
  void DelEdge(const TPair<TInt,TInt>& SrcNId, const TPair<TInt,TInt>& DstNId);
  /// Tests whether an edge between node IDs SrcNId and DstNId exists in the graph.
  bool IsEdge(const TPair<TInt,TInt>& SrcNId, const TPair<TInt,TInt>& DstNId) const;
  /// Returns an iterator referring to the first edge in the graph.
  TEdgeI BegEI() const { TNodeI NI = BegNI(); TEdgeI EI(NI, EndNI(), 0); while (NI<EndNI() && NI.GetOutDeg()==0) { EI++; } return EI; }
  /// Returns an iterator referring to the past-the-end edge in the graph.
  TEdgeI EndEI() const { return TEdgeI(EndNI(), EndNI()); }
  /// Returns an iterator referring to edge (SrcNId, DstNId) in the graph. ##TMultimodalGraphImplB::GetEI
  TEdgeI GetEI(const TPair<TInt,TInt>& SrcNId, const TPair<TInt,TInt>& DstNId) const;

  /// Transformation methods from multi-modal graph to regular, directed graph.
  TIntNNet GetSubGraph(const TIntV ModeIds) const;
  int GetSubGraphMocked(const TIntV ModeIds) const;

  /// Returns an ID of a random node in the graph.
  TPair<TInt,TInt> GetRndNId(TRnd& Rnd=TInt::Rnd) { int NId = NodeToModeMapping.GetKey(NodeToModeMapping.GetRndKeyId(Rnd, 0.8)); return TPair<TInt,TInt>(NodeToModeMapping[NId], NId); }
  /// Returns an interator referring to a random node in the graph.
  TNodeI GetRndNI(TRnd& Rnd=TInt::Rnd) { return GetNI(GetRndNId(Rnd)); }
  /// Gets a vector IDs of all nodes in the graph.
  void GetNIdV(TIntV& NIdV) const;

  /// Tests whether the graph is empty (has zero nodes).
  bool Empty() const { return GetNodes()==0; }
  /// Deletes all nodes and edges from the graph.
  void Clr();
  /// Reserves memory for a graph of Nodes nodes and Edges edges.
  void Reserve(const int& Nodes, const int& Edges) { if (Nodes>0) { NodeToModeMapping.Gen(Nodes/2); } }
  /// Defragments the graph. ##TMultimodalGraphImplB::Defrag
  void Defrag(const bool& OnlyNodeLinks=false);
  /// Checks the graph data structure for internal consistency. ##TMultimodalGraphImplB::IsOk
  bool IsOk(const bool& ThrowExcept=true) const;
  /// Returns a small graph on 10 nodes and 10 edges with 2 modes. ##TMultimodalGraphImplB::GetSmallGraph
  static PMultimodalGraphImplB GetSmallGraph();
};
