//#//////////////////////////////////////////////
/// Multimodal graph: Implementation BC
/// Single graph with all nodes [with global ids], each node labelled
/// with appropriate attribute

#ifndef MULTIMODAL_GRAPH_IMPLBC_H
#define MULTIMODAL_GRAPH_IMPLBC_H

class TMultimodalGraphImplBC;

/// Pointer to a multimodal graph
typedef TPt<TMultimodalGraphImplBC> PMultimodalGraphImplBC;

//#//////////////////////////////////////////////
/// Multimodal graph. ##TMultimodalGraphImplBC::Class
class TMultimodalGraphImplBC {
public:
  TCRef CRef;
  class TNode {
  private:
    TInt Id;
    THash<TInt,TVec<TInt> > InNIdV, OutNIdV;
  public:
    TNode() : Id(-1), InNIdV(), OutNIdV() { }
    TNode(const int& NId) : Id(NId), InNIdV(), OutNIdV() { }
    TNode(const TNode& Node) : Id(Node.Id), InNIdV(Node.InNIdV), OutNIdV(Node.OutNIdV) { }
    TNode(TSIn& SIn) : Id(SIn), InNIdV(SIn), OutNIdV(SIn) { }
    void Save(TSOut& SOut) const { Id.Save(SOut); InNIdV.Save(SOut); OutNIdV.Save(SOut); }
    int GetId() const { return Id; }
    int GetDeg() const { return GetInDeg() + GetOutDeg(); }
    // TODO: Fix these
    int GetInDeg() const;
    int GetInDeg(const int &ModeId) const {
      if (!InNIdV.IsKey(ModeId)) { return 0; }
      return InNIdV.GetDat(ModeId).Len();
    }
    int GetOutDeg() const;
    int GetOutDeg(const int &ModeId) const {
      if (!OutNIdV.IsKey(ModeId)) { return 0; }
      return OutNIdV.GetDat(ModeId).Len();
    }
    TInt GetInNId(const int& NodeN, const int& ModeId) const {
      return InNIdV.GetDat(ModeId)[NodeN];
    }
    TInt GetOutNId(const int& NodeN, const int& ModeId) const {
      return OutNIdV.GetDat(ModeId)[NodeN];
    }
    TInt GetNbrNId(const int& NodeN, const int& ModeId) const {
      return NodeN<GetOutDeg(ModeId)?GetOutNId(NodeN,ModeId):GetInNId(NodeN-GetOutDeg(ModeId),ModeId);
    }
    bool IsInNId(const TPair<TInt,TInt>& NId) const {
      if (!InNIdV.IsKey(NId.GetVal1())) { return false; }
      return InNIdV.GetDat(NId.GetVal1()).SearchBin(NId.GetVal2()) != -1;
    }
    bool IsOutNId(const TPair<TInt,TInt>& NId) const {
      if (!OutNIdV.IsKey(NId.GetVal1())) { return false; }
      return OutNIdV.GetDat(NId.GetVal1()).SearchBin(NId.GetVal2()) != -1;
    }
    bool IsNbrNId(const TPair<TInt,TInt>& NId) const { return IsOutNId(NId) || IsInNId(NId); }
    void SortNIdV();
    friend class TMultimodalGraphImplBC;
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
    int GetInDeg() const;
    int GetInDeg(const int& ModeId) const { return NodeNHI.GetDat().GetInDeg(ModeId); }
    /// Returns out-degree of the current node.
    int GetOutDeg() const;
    int GetOutDeg(const int& ModeId) const { return NodeNHI.GetDat().GetOutDeg(ModeId); }
    /// Sorts the adjacency lists of the current node.
    void SortNIdV() { NodeNHI.GetDat().SortNIdV(); }
    /// Returns ID of NodeN-th in-node (the node pointing to the current node). ##TMultimodalGraphImplBC::TNodeI::GetInNId
    TInt GetInNId(const int& NodeN, const int& ModeId) const {
      return NodeNHI.GetDat().GetInNId(NodeN, ModeId);
    }
    /// Returns ID of NodeN-th out-node (the node the current node points to). ##TMultimodalGraphImplBC::TNodeI::GetOutNId
    TInt GetOutNId(const int& NodeN, const int& ModeId) const {
      return NodeNHI.GetDat().GetOutNId(NodeN, ModeId);
    }
    /// Returns ID of NodeN-th neighboring node. ##TMultimodalGraphImplBC::TNodeI::GetNbrNId
    TInt GetNbrNId(const int& NodeN, const int& ModeId) const {
      return NodeNHI.GetDat().GetNbrNId(NodeN, ModeId);
    }
    /// Returns ModeId of current node
    int GetModeId() const { return NodeMHI.GetKey(); }
    /// Tests whether node with ID NId points to the current node.
    bool IsInNId(const TPair<TInt,TInt>& NId) const { return NodeNHI.GetDat().IsInNId(NId); }
    /// Tests whether the current node points to node with ID NId.
    bool IsOutNId(const TPair<TInt,TInt>& NId) const { return NodeNHI.GetDat().IsOutNId(NId); }
    /// Tests whether node with ID NId is a neighbor of the current node.
    bool IsNbrNId(const TPair<TInt,TInt>& NId) const { return IsOutNId(NId) || IsInNId(NId); }
    /// Get all adjacent modes.
    void GetAdjacentModes(TIntV &AdjacentModes) const;
    friend class TMultimodalGraphImplBC;
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
    TEdgeI(const TEdgeI& EdgeI) : CurNode(EdgeI.CurNode), EndNode(EdgeI.EndNode), CurEdge(EdgeI.CurEdge),
      AdjacentModes(EdgeI.AdjacentModes), CurAdjacentMode(EdgeI.CurAdjacentMode) { }
    TEdgeI& operator = (const TEdgeI& EdgeI) {
      if (this!=&EdgeI) {
        CurNode=EdgeI.CurNode; EndNode=EdgeI.EndNode; CurEdge=EdgeI.CurEdge;
        AdjacentModes=EdgeI.AdjacentModes; CurAdjacentMode=EdgeI.CurAdjacentMode;
      }
      return *this;
    }
    /// Increment iterator.
    TEdgeI& operator++ (int) {
      CurEdge++;
      if (CurEdge >= CurNode.GetOutDeg(AdjacentModes.GetVal(CurAdjacentMode))) {
        CurEdge=0; CurAdjacentMode++;
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
    bool operator < (const TEdgeI& EdgeI) const {
      return CurNode<EdgeI.CurNode ||
        (CurNode==EdgeI.CurNode && CurAdjacentMode<EdgeI.CurAdjacentMode) ||
        (CurNode==EdgeI.CurNode && CurAdjacentMode==EdgeI.CurAdjacentMode && CurEdge<EdgeI.CurEdge);
    }
    bool operator == (const TEdgeI& EdgeI) const { return CurNode == EdgeI.CurNode && CurEdge == EdgeI.CurEdge && CurAdjacentMode == EdgeI.CurAdjacentMode; }
    /// Returns edge ID. Always returns -1 since only edges in multigraphs have explicit IDs.
    int GetId() const { return -1; }
    /// Returns the source node of the edge.
    TPair<TInt,TInt> GetSrcNId() const { return CurNode.GetId(); }
    /// Returns the destination node of the edge.
    TPair<TInt,TInt> GetDstNId() const {
      int CurMode = AdjacentModes.GetVal(CurAdjacentMode);
      return TPair<TInt,TInt>(CurMode, CurNode.GetOutNId(CurEdge, CurMode));
    }
    /// Returns NodeN of current edge.
    int GetCurEdge() const { return CurEdge; }
    friend class TMultimodalGraphImplBC;
  };
private:
  TInt MxNId, NEdges;
  // Encapsulate a network under the hood
  typedef THash< TInt,THash<TInt,TNode> > TNodeH;
  TNodeH NodeH;
public:
  TMultimodalGraphImplBC() : CRef(), MxNId(0), NEdges(0), NodeH() { }
  /// Constructor that reserves enough memory for a graph of Nodes nodes and Edges edges.
  explicit TMultimodalGraphImplBC(const int& Nodes, const int& Edges) : CRef(), MxNId(0), NEdges(0) { Reserve(Nodes, Edges); }
  TMultimodalGraphImplBC(const TMultimodalGraphImplBC& Graph) : MxNId(Graph.MxNId), NEdges(Graph.NEdges), NodeH(Graph.NodeH) { }
  /// Constructor that loads the graph from a (binary) stream SIn.
  TMultimodalGraphImplBC(TSIn& SIn) : MxNId(SIn), NEdges(SIn), NodeH(NodeH) { }
  virtual ~TMultimodalGraphImplBC() { }
  /// Saves the graph to a (binary) stream SOut.
  void Save(TSOut& SOut) const { MxNId.Save(SOut); NEdges.Save(SOut); NodeH.Save(SOut); }
  /// Static constructor that returns a pointer to the graph. Call: PMultimodalGraphImplBC Graph = TMultimodalGraphImplBCGraph::New().
  static PMultimodalGraphImplBC New() { return new TMultimodalGraphImplBC(); }
  /// Static constructor that returns a pointer to the graph and reserves enough memory for Nodes nodes and Edges edges. ##TMultimodalGraphImplBC::New
  static PMultimodalGraphImplBC New(const int& Nodes, const int& Edges) { return new TMultimodalGraphImplBC(Nodes, Edges); }
  /// Static constructor that loads the graph from a stream SIn and returns a pointer to it.
  static PMultimodalGraphImplBC Load(TSIn& SIn) { return PMultimodalGraphImplBC(new TMultimodalGraphImplBC(SIn)); }
  /// Allows for run-time checking the type of the graph (see the TGraphFlag for flags).
  // TODO: Implement this
  bool HasFlag(const TGraphFlag& Flag) const;
  TMultimodalGraphImplBC& operator = (const TMultimodalGraphImplBC& Graph) {
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
  void RandomWalk(TVec< TPair<TInt,TInt> > NodeIds, int WalkLength);

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
  /// Defragments the graph. ##TUNGraph::Defrag
  void Defrag(const bool& OnlyNodeLinks=false);
  /// Checks the graph data structure for internal consistency. ##TUNGraph::IsOk
  bool IsOk(const bool& ThrowExcept=true) const;
  /// Returns a small graph on 10 nodes and 10 edges with 2 modes. ##TUNGraph::GetSmallGraph
  static PMultimodalGraphImplBC GetSmallGraph();
};

#endif
