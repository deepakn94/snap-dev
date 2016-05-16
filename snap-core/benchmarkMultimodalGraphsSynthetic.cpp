// Benchmarking simple multimodal graph functionality

#include "Snap.h"

void CreateGraphMapping(THash<TInt, TInt>& nodeToModeMapping,
                        THash<TInt,TVec<TInt> >& edges,
                        TVec<TInt>& verticesToBeDeleted,
                        TVec< TPair<TInt,TInt> >& edgesToBeDeleted,
                        TVec< TPair<TInt,TInt> >& verticesToBeChecked,
                        TVec<TInt>& extraVerticesToBeChecked,
                        TVec< TPair<TInt,TInt> >& edgesSrcVertexToBeChecked,
                        TVec< TPair<TInt,TInt> >& edgesDstVertexToBeChecked,
                        TVec< TPair<TInt,TInt> >& extraEdgesToBeChecked) {
  int numModes = 10;
  int numNodesPerMode = 1000000;
  int numNodes = numNodesPerMode * numModes;
  long int numEdges = 0;

  for (int i = 0; i < numNodesPerMode; i++) {
    for (int j = 0; j < numModes; j++) {
      nodeToModeMapping.AddDat((j*numNodesPerMode)+i, j);
    }
  }

  for (int i = 0; i < numNodes; i++) {
    edges.AddDat(i);
    if (i % 10000 != 0) { edges.GetDat(i).Add((i+5) % numNodes); numEdges++; continue; }
    int numEdgesPerNode = 1000000;
    for (int j = 0; j < numEdgesPerNode; j++) {
      edges.GetDat(i).Add((i+(j*(numNodes / numEdgesPerNode))) % numNodes);
      numEdges++;
    }
  }

  printf("Total number of nodes: %d...\n", numNodes);
  printf("Total number of edges: %ld...\n", numEdges);

  int numNodesToBeDeleted = 0.3 * numNodes;
  printf("Number of nodes to be deleted: %d...\n", numNodesToBeDeleted);
  for (int i = 0; i < numNodesToBeDeleted; i++) {
    TInt vertexToBeDeleted = nodeToModeMapping.GetKey(nodeToModeMapping.GetRndKeyId(TInt::Rnd));
    while (verticesToBeDeleted.IsIn(vertexToBeDeleted)) {
      vertexToBeDeleted = nodeToModeMapping.GetKey(nodeToModeMapping.GetRndKeyId(TInt::Rnd));
    }
    verticesToBeDeleted.Add(vertexToBeDeleted);
  }

  for (int i = 0; i < 1000000; i++) {
    TInt vertexToBeChecked = nodeToModeMapping.GetKey(nodeToModeMapping.GetRndKeyId(TInt::Rnd));
    extraVerticesToBeChecked.Add(vertexToBeChecked);
  }

  for (int i = 0; i < 1000000; i++) {
    TInt srcVertex = edges.GetKey(edges.GetRndKeyId(TInt::Rnd));
    TInt dstVertexIndex = TInt::Rnd.GetUniDevInt(edges.GetDat(srcVertex).Len());
    extraEdgesToBeChecked.Add(TPair<TInt,TInt>(srcVertex,dstVertexIndex));
  }

  int numEdgesToBeDeleted = (numEdges / 100000);
  printf("Number of edges to be deleted: %d...\n", numEdgesToBeDeleted);
  for (int i = 0; i < numEdgesToBeDeleted; i++) {
    TInt srcVertex = edges.GetKey(edges.GetRndKeyId(TInt::Rnd));
    TInt dstVertex = edges.GetDat(srcVertex).GetRndVal();
    TPair<TInt,TInt> edgeToBeDeleted = TPair<TInt,TInt>(srcVertex,dstVertex);
    while (edgesToBeDeleted.IsIn(edgeToBeDeleted) ||
           verticesToBeDeleted.IsIn(edgeToBeDeleted.GetVal1()) ||
           verticesToBeDeleted.IsIn(edgeToBeDeleted.GetVal2())) {
      srcVertex = edges.GetKey(edges.GetRndKeyId(TInt::Rnd));
      dstVertex = edges.GetDat(srcVertex).GetRndVal();
      edgeToBeDeleted = TPair<TInt,TInt>(srcVertex,dstVertex);
    }
    edgesToBeDeleted.Add(edgeToBeDeleted);
  }

  for (int i = 0; i < 10000000; i++) {
    verticesToBeChecked.Add(TPair<TInt,TInt>(TInt::Rnd.GetUniDevInt(numModes), TInt::Rnd.GetUniDevInt(100000000)));
  }

  for (int i = 0; i < 10000000; i++) {
    edgesSrcVertexToBeChecked.Add(verticesToBeChecked.GetRndVal());
    edgesDstVertexToBeChecked.Add(verticesToBeChecked.GetRndVal());
  }
}

void testImplementationA(const THash<TInt,TInt>& nodeToModeMapping,
                         const THash<TInt,TVec<TInt> >& edges,
                         const TVec<TInt>& verticesToBeDeleted,
                         const TVec< TPair<TInt,TInt> >& edgesToBeDeleted,
                         const TVec< TPair<TInt,TInt> >& verticesToBeChecked,
                         const TVec<TInt>& extraVerticesToBeChecked,
                         const TVec< TPair<TInt,TInt> >& edgesSrcVertexToBeChecked,
                         const TVec< TPair<TInt,TInt> >& edgesDstVertexToBeChecked,
                         const TVec< TPair<TInt,TInt> >& extraEdgesToBeChecked) {
  PMultimodalGraphImplA G = TMultimodalGraphImplA::New();
  printf("Creating graph, implementation A...\n");
  double totalLoadTime = 0.0;

  TStopwatch* sw = TStopwatch::GetInstance();
  sw->Start(TStopwatch::LoadTables);
  THash<TInt, TPair<TInt,TInt> > GraphIdToNodeIdMapping = THash<TInt, TPair<TInt,TInt> >();
  for (THash<TInt,TInt>::TIter CurI = nodeToModeMapping.BegI(); CurI < nodeToModeMapping.EndI(); CurI++) {
    GraphIdToNodeIdMapping.AddDat(CurI.GetKey(), G->AddNode(CurI.GetDat()));
  }
  sw->Stop(TStopwatch::LoadTables);
  totalLoadTime += sw->Last(TStopwatch::LoadTables);

  THash<TInt,TVec<TPair<TInt,TInt> > > edgesProcessed = THash<TInt,TVec<TPair<TInt,TInt> > >();
  for (THash<TInt,TVec<TInt> >::TIter CurI = edges.BegI(); CurI < edges.EndI(); CurI++) {
    edgesProcessed.AddDat(CurI.GetKey());
    for (TVec<TInt>::TIter CurIEdge = CurI.GetDat().BegI(); CurIEdge < CurI.GetDat().EndI(); CurIEdge++) {
      edgesProcessed.GetDat(CurI.GetKey()).Add(GraphIdToNodeIdMapping.GetDat(*CurIEdge));
    }
  }

  sw->Start(TStopwatch::LoadTables);
  for (THash<TInt,TVec<TPair<TInt,TInt> > >::TIter CurI = edgesProcessed.BegI(); CurI < edgesProcessed.EndI(); CurI++) {
    G->AddEdgeBatch(GraphIdToNodeIdMapping.GetDat(CurI.GetKey()), CurI.GetDat());
  }
  sw->Stop(TStopwatch::LoadTables);
  totalLoadTime += sw->Last(TStopwatch::LoadTables);
  printf("Total time taken creating graph: %.2f seconds\n", totalLoadTime);

  TIntV Modes1 = TIntV(); Modes1.Add(0); Modes1.Add(1);
  TIntV Modes2 = TIntV(); Modes2.Add(0); Modes2.Add(5);
  TIntV Modes3 = TIntV(); Modes3.Add(0); Modes3.Add(1); Modes3.Add(2);
  TIntV Modes4 = TIntV(); Modes4.Add(5);
  TIntV Modes5 = TIntV(); Modes5.Add(0); Modes5.Add(1); Modes5.Add(4);
  TIntV Modes6 = TIntV(); for (int i = 0; i < 38; i++) { Modes6.Add(i); }

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraphMocked(Modes1);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 0 and 1: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraphMocked(Modes2);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 0 and 5: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraphMocked(Modes3);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 0, 1 and 2: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraphMocked(Modes4);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 5: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraphMocked(Modes5);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 0, 1 and 4: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraphMocked(Modes6);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 0 to 37: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

  sw->Start(TStopwatch::EstimateSizes);
  TVec< TPair<TInt,TInt> > PerModeNodeIds = TVec< TPair<TInt,TInt> >();
  G->GetNodeIdsInMode(0, PerModeNodeIds);
  G->GetNodeIdsInMode(1, PerModeNodeIds);
  G->GetNodeIdsInMode(3, PerModeNodeIds);
  sw->Stop(TStopwatch::EstimateSizes);
  printf("Total time taken getting node ids in modes 0, 1 and 3: %.2f seconds\n", sw->Last(TStopwatch::EstimateSizes));

  sw->Start(TStopwatch::EstimateSizes);
  for (int i = 0; i < verticesToBeChecked.Len(); i++) {
    G->IsNode(verticesToBeChecked.GetVal(i));
  }
  sw->Stop(TStopwatch::EstimateSizes);
  printf("Total time taken determining if nodes are not in graph: %.2f seconds\n", sw->Last(TStopwatch::EstimateSizes));

  sw->Start(TStopwatch::EstimateSizes);
  for (int i = 0; i < extraVerticesToBeChecked.Len(); i++) {
    G->IsNode(GraphIdToNodeIdMapping.GetDat(extraVerticesToBeChecked.GetVal(i)));
  }
  sw->Stop(TStopwatch::EstimateSizes);
  printf("Total time taken determining if nodes are in graph: %.2f seconds\n", sw->Last(TStopwatch::EstimateSizes));

  sw->Start(TStopwatch::EstimateSizes);
  for (int i = 0; i < edgesSrcVertexToBeChecked.Len(); i++) {
    G->IsEdge(edgesSrcVertexToBeChecked.GetVal(i), edgesDstVertexToBeChecked.GetVal(i));
  }
  sw->Stop(TStopwatch::EstimateSizes);
  printf("Total time taken determining if edges are not in graph: %.2f seconds\n", sw->Last(TStopwatch::EstimateSizes));

  sw->Start(TStopwatch::EstimateSizes);
  for (int i = 0; i < extraEdgesToBeChecked.Len(); i++) {
    TPair<TInt,TInt> edge = extraEdgesToBeChecked.GetVal(i);
    TPair<TInt,TInt> edgeToBeChecked = TPair<TInt,TInt>(edge.GetVal1(),
      edges.GetDat(edge.GetVal1()).GetVal(edge.GetVal2()));
    G->IsEdge(GraphIdToNodeIdMapping.GetDat(edgeToBeChecked.GetVal1()),
              GraphIdToNodeIdMapping.GetDat(edgeToBeChecked.GetVal2()));
  }
  sw->Stop(TStopwatch::EstimateSizes);
  printf("Total time taken determining if edges are in graph: %.2f seconds\n", sw->Last(TStopwatch::EstimateSizes));

  sw->Start(TStopwatch::RemoveEdges);
  for (TVec<TInt>::TIter CurI = verticesToBeDeleted.BegI(); CurI < verticesToBeDeleted.EndI(); CurI++) {
    G->DelNode(GraphIdToNodeIdMapping.GetDat(*CurI));
  }

  for (TVec< TPair<TInt,TInt> >::TIter CurI = edgesToBeDeleted.BegI(); CurI < edgesToBeDeleted.EndI(); CurI++) {
    G->DelEdge(GraphIdToNodeIdMapping.GetDat(CurI->GetVal1()), GraphIdToNodeIdMapping.GetDat(CurI->GetVal2()));
  }
  sw->Stop(TStopwatch::RemoveEdges);
  printf("Total time taken removing edges: %.2f seconds\n", sw->Last(TStopwatch::RemoveEdges));

  sw->Start(TStopwatch::AddEdges);
  for (TVec<TInt>::TIter CurI = verticesToBeDeleted.BegI(); CurI < verticesToBeDeleted.EndI(); CurI++) {
    G->AddNode(GraphIdToNodeIdMapping.GetDat(*CurI));
  }

  for (TVec< TPair<TInt,TInt> >::TIter CurI = edgesToBeDeleted.BegI(); CurI < edgesToBeDeleted.EndI(); CurI++) {
    G->AddEdge(GraphIdToNodeIdMapping.GetDat(CurI->GetVal1()), GraphIdToNodeIdMapping.GetDat(CurI->GetVal2()));
  }
  sw->Stop(TStopwatch::AddEdges);
  printf("Total time taken re-adding edges: %.2f seconds\n\n", sw->Last(TStopwatch::AddEdges));
}

void testImplementationB(const THash<TInt,TInt>& nodeToModeMapping,
                         const THash<TInt,TVec<TInt> >& edges,
                         const TVec<TInt>& verticesToBeDeleted,
                         const TVec< TPair<TInt,TInt> >& edgesToBeDeleted,
                         const TVec< TPair<TInt,TInt> >& verticesToBeChecked,
                         const TVec<TInt>& extraVerticesToBeChecked,
                         const TVec< TPair<TInt,TInt> >& edgesSrcVertexToBeChecked,
                         const TVec< TPair<TInt,TInt> >& edgesDstVertexToBeChecked,
                         const TVec< TPair<TInt,TInt> >& extraEdgesToBeChecked) {
  PMultimodalGraphImplB G = TMultimodalGraphImplB::New();
  printf("Creating graph, implementation B...\n");
  double totalLoadTime = 0.0;

  TStopwatch* sw = TStopwatch::GetInstance();
  sw->Start(TStopwatch::LoadTables);
  THash<TInt, TPair<TInt,TInt> > GraphIdToNodeIdMapping = THash<TInt, TPair<TInt,TInt> >();
  for (THash<TInt,TInt>::TIter CurI = nodeToModeMapping.BegI(); CurI < nodeToModeMapping.EndI(); CurI++) {
    GraphIdToNodeIdMapping.AddDat(CurI.GetKey(), G->AddNode(CurI.GetDat()));
  }
  sw->Stop(TStopwatch::LoadTables);
  totalLoadTime += sw->Last(TStopwatch::LoadTables);

  THash<TInt,TVec<TPair<TInt,TInt> > > edgesProcessed = THash<TInt,TVec<TPair<TInt,TInt> > >();
  for (THash<TInt,TVec<TInt> >::TIter CurI = edges.BegI(); CurI < edges.EndI(); CurI++) {
    edgesProcessed.AddDat(CurI.GetKey());
    for (TVec<TInt>::TIter CurIEdge = CurI.GetDat().BegI(); CurIEdge < CurI.GetDat().EndI(); CurIEdge++) {
      edgesProcessed.GetDat(CurI.GetKey()).Add(GraphIdToNodeIdMapping.GetDat(*CurIEdge));
    }
  }

  sw->Start(TStopwatch::LoadTables);
  for (THash<TInt,TVec<TPair<TInt,TInt> > >::TIter CurI = edgesProcessed.BegI(); CurI < edgesProcessed.EndI(); CurI++) {
    G->AddEdgeBatch(GraphIdToNodeIdMapping.GetDat(CurI.GetKey()), CurI.GetDat());
  }
  sw->Stop(TStopwatch::LoadTables);
  totalLoadTime += sw->Last(TStopwatch::LoadTables);
  printf("Total time taken creating graph: %.2f seconds\n", totalLoadTime);

  TIntV Modes1 = TIntV(); Modes1.Add(0); Modes1.Add(1);
  TIntV Modes2 = TIntV(); Modes2.Add(0); Modes2.Add(5);
  TIntV Modes3 = TIntV(); Modes3.Add(0); Modes3.Add(1); Modes3.Add(2);
  TIntV Modes4 = TIntV(); Modes4.Add(5);
  TIntV Modes5 = TIntV(); Modes5.Add(0); Modes5.Add(1); Modes5.Add(4);
  TIntV Modes6 = TIntV(); for (int i = 0; i < 38; i++) { Modes6.Add(i); }

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraphMocked(Modes1);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 0 and 1: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraphMocked(Modes2);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 0 and 5: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraphMocked(Modes3);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 0, 1 and 2: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraphMocked(Modes4);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 5: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraphMocked(Modes5);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 0, 1 and 4: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraphMocked(Modes6);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 0 to 37: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

  sw->Start(TStopwatch::EstimateSizes);
  TVec< TPair<TInt,TInt> > PerModeNodeIds = TVec< TPair<TInt,TInt> >();
  G->GetNodeIdsInMode(0, PerModeNodeIds);
  G->GetNodeIdsInMode(1, PerModeNodeIds);
  G->GetNodeIdsInMode(3, PerModeNodeIds);
  sw->Stop(TStopwatch::EstimateSizes);
  printf("Total time taken getting node ids in modes 0, 1 and 3: %.2f seconds\n", sw->Last(TStopwatch::EstimateSizes));

  sw->Start(TStopwatch::EstimateSizes);
  for (int i = 0; i < verticesToBeChecked.Len(); i++) {
    G->IsNode(verticesToBeChecked.GetVal(i));
  }
  sw->Stop(TStopwatch::EstimateSizes);
  printf("Total time taken determining if nodes are not in graph: %.2f seconds\n", sw->Last(TStopwatch::EstimateSizes));

  sw->Start(TStopwatch::EstimateSizes);
  for (int i = 0; i < extraVerticesToBeChecked.Len(); i++) {
    G->IsNode(GraphIdToNodeIdMapping.GetDat(extraVerticesToBeChecked.GetVal(i)));
  }
  sw->Stop(TStopwatch::EstimateSizes);
  printf("Total time taken determining if nodes are in graph: %.2f seconds\n", sw->Last(TStopwatch::EstimateSizes));

  sw->Start(TStopwatch::EstimateSizes);
  for (int i = 0; i < edgesSrcVertexToBeChecked.Len(); i++) {
    G->IsEdge(edgesSrcVertexToBeChecked.GetVal(i), edgesDstVertexToBeChecked.GetVal(i));
  }
  sw->Stop(TStopwatch::EstimateSizes);
  printf("Total time taken determining if edges are not in graph: %.2f seconds\n", sw->Last(TStopwatch::EstimateSizes));

  sw->Start(TStopwatch::EstimateSizes);
  for (int i = 0; i < extraEdgesToBeChecked.Len(); i++) {
    TPair<TInt,TInt> edge = extraEdgesToBeChecked.GetVal(i);
    TPair<TInt,TInt> edgeToBeChecked = TPair<TInt,TInt>(edge.GetVal1(),
      edges.GetDat(edge.GetVal1()).GetVal(edge.GetVal2()));
    G->IsEdge(GraphIdToNodeIdMapping.GetDat(edgeToBeChecked.GetVal1()),
              GraphIdToNodeIdMapping.GetDat(edgeToBeChecked.GetVal2()));
  }
  sw->Stop(TStopwatch::EstimateSizes);
  printf("Total time taken determining if edges are in graph: %.2f seconds\n", sw->Last(TStopwatch::EstimateSizes));

  sw->Start(TStopwatch::RemoveEdges);
  for (TVec<TInt>::TIter CurI = verticesToBeDeleted.BegI(); CurI < verticesToBeDeleted.EndI(); CurI++) {
    G->DelNode(GraphIdToNodeIdMapping.GetDat(*CurI));
  }

  for (TVec< TPair<TInt,TInt> >::TIter CurI = edgesToBeDeleted.BegI(); CurI < edgesToBeDeleted.EndI(); CurI++) {
    G->DelEdge(GraphIdToNodeIdMapping.GetDat(CurI->GetVal1()), GraphIdToNodeIdMapping.GetDat(CurI->GetVal2()));
  }
  sw->Stop(TStopwatch::RemoveEdges);
  printf("Total time taken removing edges: %.2f seconds\n\n", sw->Last(TStopwatch::RemoveEdges));

  sw->Start(TStopwatch::AddEdges);
  for (TVec<TInt>::TIter CurI = verticesToBeDeleted.BegI(); CurI < verticesToBeDeleted.EndI(); CurI++) {
    G->AddNode(GraphIdToNodeIdMapping.GetDat(*CurI));
  }

  for (TVec< TPair<TInt,TInt> >::TIter CurI = edgesToBeDeleted.BegI(); CurI < edgesToBeDeleted.EndI(); CurI++) {
    G->AddEdge(GraphIdToNodeIdMapping.GetDat(CurI->GetVal1()), GraphIdToNodeIdMapping.GetDat(CurI->GetVal2()));
  }
  sw->Stop(TStopwatch::AddEdges);
  printf("Total time taken re-adding edges: %.2f seconds\n\n", sw->Last(TStopwatch::AddEdges));
}

void testImplementationC(const THash<TInt,TInt>& nodeToModeMapping,
                         const THash<TInt,TVec<TInt> >& edges,
                         const TVec<TInt>& verticesToBeDeleted,
                         const TVec< TPair<TInt,TInt> >& edgesToBeDeleted,
                         const TVec< TPair<TInt,TInt> >& verticesToBeChecked,
                         const TVec<TInt>& extraVerticesToBeChecked,
                         const TVec< TPair<TInt,TInt> >& edgesSrcVertexToBeChecked,
                         const TVec< TPair<TInt,TInt> >& edgesDstVertexToBeChecked,
                         const TVec< TPair<TInt,TInt> >& extraEdgesToBeChecked) {
  PMultimodalGraphImplC G = TMultimodalGraphImplC::New();
  printf("Creating graph, implementation C...\n");
  double totalLoadTime = 0.0;

  TStopwatch* sw = TStopwatch::GetInstance();
  sw->Start(TStopwatch::LoadTables);
  THash<TInt, TPair<TInt,TInt> > GraphIdToNodeIdMapping = THash<TInt, TPair<TInt,TInt> >();
  for (THash<TInt,TInt>::TIter CurI = nodeToModeMapping.BegI(); CurI < nodeToModeMapping.EndI(); CurI++) {
    GraphIdToNodeIdMapping.AddDat(CurI.GetKey(), G->AddNode(CurI.GetDat()));
  }
  sw->Stop(TStopwatch::LoadTables);
  totalLoadTime += sw->Last(TStopwatch::LoadTables);

  THash<TInt,TVec<TPair<TInt,TInt> > > edgesProcessed = THash<TInt,TVec<TPair<TInt,TInt> > >();
  for (THash<TInt,TVec<TInt> >::TIter CurI = edges.BegI(); CurI < edges.EndI(); CurI++) {
    edgesProcessed.AddDat(CurI.GetKey());
    for (TVec<TInt>::TIter CurIEdge = CurI.GetDat().BegI(); CurIEdge < CurI.GetDat().EndI(); CurIEdge++) {
      edgesProcessed.GetDat(CurI.GetKey()).Add(GraphIdToNodeIdMapping.GetDat(*CurIEdge));
    }
  }

  sw->Start(TStopwatch::LoadTables);
  for (THash<TInt,TVec<TPair<TInt,TInt> > >::TIter CurI = edgesProcessed.BegI(); CurI < edgesProcessed.EndI(); CurI++) {
    G->AddEdgeBatch(GraphIdToNodeIdMapping.GetDat(CurI.GetKey()), CurI.GetDat());
  }
  sw->Stop(TStopwatch::LoadTables);
  totalLoadTime += sw->Last(TStopwatch::LoadTables);
  printf("Total time taken creating graph: %.2f seconds\n", totalLoadTime);

  TIntV Modes1 = TIntV(); Modes1.Add(0); Modes1.Add(1);
  TIntV Modes2 = TIntV(); Modes2.Add(0); Modes2.Add(5);
  TIntV Modes3 = TIntV(); Modes3.Add(0); Modes3.Add(1); Modes3.Add(2);
  TIntV Modes4 = TIntV(); Modes4.Add(5);
  TIntV Modes5 = TIntV(); Modes5.Add(0); Modes5.Add(1); Modes5.Add(4);
  TIntV Modes6 = TIntV(); for (int i = 0; i < 38; i++) { Modes6.Add(i); }

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraphMocked(Modes1);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 0 and 1: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraphMocked(Modes2);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 0 and 5: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraphMocked(Modes3);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 0, 1 and 2: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraphMocked(Modes4);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 5: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraphMocked(Modes5);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 0, 1 and 4: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraphMocked(Modes6);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 0 to 37: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

  sw->Start(TStopwatch::EstimateSizes);
  TVec< TPair<TInt,TInt> > PerModeNodeIds = TVec< TPair<TInt,TInt> >();
  G->GetNodeIdsInMode(0, PerModeNodeIds);
  G->GetNodeIdsInMode(1, PerModeNodeIds);
  G->GetNodeIdsInMode(3, PerModeNodeIds);
  sw->Stop(TStopwatch::EstimateSizes);
  printf("Total time taken getting node ids in modes 0, 1 and 3: %.2f seconds\n", sw->Last(TStopwatch::EstimateSizes));

  sw->Start(TStopwatch::EstimateSizes);
  for (int i = 0; i < verticesToBeChecked.Len(); i++) {
    G->IsNode(verticesToBeChecked.GetVal(i));
  }
  sw->Stop(TStopwatch::EstimateSizes);
  printf("Total time taken determining if nodes are not in graph: %.2f seconds\n", sw->Last(TStopwatch::EstimateSizes));

  sw->Start(TStopwatch::EstimateSizes);
  for (int i = 0; i < extraVerticesToBeChecked.Len(); i++) {
    G->IsNode(GraphIdToNodeIdMapping.GetDat(extraVerticesToBeChecked.GetVal(i)));
  }
  sw->Stop(TStopwatch::EstimateSizes);
  printf("Total time taken determining if nodes are in graph: %.2f seconds\n", sw->Last(TStopwatch::EstimateSizes));

  sw->Start(TStopwatch::EstimateSizes);
  for (int i = 0; i < edgesSrcVertexToBeChecked.Len(); i++) {
    G->IsEdge(edgesSrcVertexToBeChecked.GetVal(i), edgesDstVertexToBeChecked.GetVal(i));
  }
  sw->Stop(TStopwatch::EstimateSizes);
  printf("Total time taken determining if edges are not in graph: %.2f seconds\n", sw->Last(TStopwatch::EstimateSizes));

  sw->Start(TStopwatch::EstimateSizes);
  for (int i = 0; i < extraEdgesToBeChecked.Len(); i++) {
    TPair<TInt,TInt> edge = extraEdgesToBeChecked.GetVal(i);
    TPair<TInt,TInt> edgeToBeChecked = TPair<TInt,TInt>(edge.GetVal1(),
      edges.GetDat(edge.GetVal1()).GetVal(edge.GetVal2()));
    G->IsEdge(GraphIdToNodeIdMapping.GetDat(edgeToBeChecked.GetVal1()),
              GraphIdToNodeIdMapping.GetDat(edgeToBeChecked.GetVal2()));
  }
  sw->Stop(TStopwatch::EstimateSizes);
  printf("Total time taken determining if edges are in graph: %.2f seconds\n", sw->Last(TStopwatch::EstimateSizes));

  sw->Start(TStopwatch::RemoveEdges);
  for (TVec<TInt>::TIter CurI = verticesToBeDeleted.BegI(); CurI < verticesToBeDeleted.EndI(); CurI++) {
    G->DelNode(GraphIdToNodeIdMapping.GetDat(*CurI));
  }

  for (TVec< TPair<TInt,TInt> >::TIter CurI = edgesToBeDeleted.BegI(); CurI < edgesToBeDeleted.EndI(); CurI++) {
    G->DelEdge(GraphIdToNodeIdMapping.GetDat(CurI->GetVal1()), GraphIdToNodeIdMapping.GetDat(CurI->GetVal2()));
  }
  sw->Stop(TStopwatch::RemoveEdges);
  printf("Total time taken removing edges: %.2f seconds\n\n", sw->Last(TStopwatch::RemoveEdges));

  sw->Start(TStopwatch::AddEdges);
  for (TVec<TInt>::TIter CurI = verticesToBeDeleted.BegI(); CurI < verticesToBeDeleted.EndI(); CurI++) {
    G->AddNode(GraphIdToNodeIdMapping.GetDat(*CurI));
  }

  for (TVec< TPair<TInt,TInt> >::TIter CurI = edgesToBeDeleted.BegI(); CurI < edgesToBeDeleted.EndI(); CurI++) {
    G->AddEdge(GraphIdToNodeIdMapping.GetDat(CurI->GetVal1()), GraphIdToNodeIdMapping.GetDat(CurI->GetVal2()));
  }
  sw->Stop(TStopwatch::AddEdges);
  printf("Total time taken re-adding edges: %.2f seconds\n\n", sw->Last(TStopwatch::AddEdges));
}

void testImplementationBC(const THash<TInt,TInt>& nodeToModeMapping,
                          const THash<TInt,TVec<TInt> >& edges,
                          const TVec<TInt>& verticesToBeDeleted,
                          const TVec< TPair<TInt,TInt> >& edgesToBeDeleted,
                          const TVec< TPair<TInt,TInt> >& verticesToBeChecked,
                          const TVec<TInt>& extraVerticesToBeChecked,
                          const TVec< TPair<TInt,TInt> >& edgesSrcVertexToBeChecked,
                          const TVec< TPair<TInt,TInt> >& edgesDstVertexToBeChecked,
                          const TVec< TPair<TInt,TInt> >& extraEdgesToBeChecked) {
  PMultimodalGraphImplBC G = TMultimodalGraphImplBC::New();
  printf("Creating graph, implementation BC...\n");
  double totalLoadTime = 0.0;

  TStopwatch* sw = TStopwatch::GetInstance();
  sw->Start(TStopwatch::LoadTables);
  THash<TInt, TPair<TInt,TInt> > GraphIdToNodeIdMapping = THash<TInt, TPair<TInt,TInt> >();
  for (THash<TInt,TInt>::TIter CurI = nodeToModeMapping.BegI(); CurI < nodeToModeMapping.EndI(); CurI++) {
    GraphIdToNodeIdMapping.AddDat(CurI.GetKey(), G->AddNode(CurI.GetDat()));
  }
  sw->Stop(TStopwatch::LoadTables);
  totalLoadTime += sw->Last(TStopwatch::LoadTables);

  THash<TInt,TVec<TPair<TInt,TInt> > > edgesProcessed = THash<TInt,TVec<TPair<TInt,TInt> > >();
  for (THash<TInt,TVec<TInt> >::TIter CurI = edges.BegI(); CurI < edges.EndI(); CurI++) {
    edgesProcessed.AddDat(CurI.GetKey());
    for (TVec<TInt>::TIter CurIEdge = CurI.GetDat().BegI(); CurIEdge < CurI.GetDat().EndI(); CurIEdge++) {
      edgesProcessed.GetDat(CurI.GetKey()).Add(GraphIdToNodeIdMapping.GetDat(*CurIEdge));
    }
  }

  sw->Start(TStopwatch::LoadTables);
  for (THash<TInt,TVec<TPair<TInt,TInt> > >::TIter CurI = edgesProcessed.BegI(); CurI < edgesProcessed.EndI(); CurI++) {
    G->AddEdgeBatch(GraphIdToNodeIdMapping.GetDat(CurI.GetKey()), CurI.GetDat());
  }
  sw->Stop(TStopwatch::LoadTables);
  totalLoadTime += sw->Last(TStopwatch::LoadTables);
  printf("Total time taken creating graph: %.2f seconds\n", totalLoadTime);

  TIntV Modes1 = TIntV(); Modes1.Add(0); Modes1.Add(1);
  TIntV Modes2 = TIntV(); Modes2.Add(0); Modes2.Add(5);
  TIntV Modes3 = TIntV(); Modes3.Add(0); Modes3.Add(1); Modes3.Add(2);
  TIntV Modes4 = TIntV(); Modes4.Add(5);
  TIntV Modes5 = TIntV(); Modes5.Add(0); Modes5.Add(1); Modes5.Add(4);
  TIntV Modes6 = TIntV(); for (int i = 0; i < 38; i++) { Modes6.Add(i); }

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraphMocked(Modes1);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 0 and 1: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraphMocked(Modes2);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 0 and 5: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraphMocked(Modes3);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 0, 1 and 2: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraphMocked(Modes4);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 5: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraphMocked(Modes5);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 0, 1 and 4: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraphMocked(Modes6);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 0 to 37: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

  sw->Start(TStopwatch::EstimateSizes);
  TVec< TPair<TInt,TInt> > PerModeNodeIds = TVec< TPair<TInt,TInt> >();
  G->GetNodeIdsInMode(0, PerModeNodeIds);
  G->GetNodeIdsInMode(1, PerModeNodeIds);
  G->GetNodeIdsInMode(3, PerModeNodeIds);
  sw->Stop(TStopwatch::EstimateSizes);
  printf("Total time taken getting node ids in modes 0, 1 and 3: %.2f seconds\n", sw->Last(TStopwatch::EstimateSizes));

  sw->Start(TStopwatch::EstimateSizes);
  for (int i = 0; i < verticesToBeChecked.Len(); i++) {
    G->IsNode(verticesToBeChecked.GetVal(i));
  }
  sw->Stop(TStopwatch::EstimateSizes);
  printf("Total time taken determining if nodes are not in graph: %.2f seconds\n", sw->Last(TStopwatch::EstimateSizes));

  sw->Start(TStopwatch::EstimateSizes);
  for (int i = 0; i < extraVerticesToBeChecked.Len(); i++) {
    G->IsNode(GraphIdToNodeIdMapping.GetDat(extraVerticesToBeChecked.GetVal(i)));
  }
  sw->Stop(TStopwatch::EstimateSizes);
  printf("Total time taken determining if nodes are in graph: %.2f seconds\n", sw->Last(TStopwatch::EstimateSizes));

  sw->Start(TStopwatch::EstimateSizes);
  for (int i = 0; i < edgesSrcVertexToBeChecked.Len(); i++) {
    G->IsEdge(edgesSrcVertexToBeChecked.GetVal(i), edgesDstVertexToBeChecked.GetVal(i));
  }
  sw->Stop(TStopwatch::EstimateSizes);
  printf("Total time taken determining if edges are not in graph: %.2f seconds\n", sw->Last(TStopwatch::EstimateSizes));

  sw->Start(TStopwatch::EstimateSizes);
  for (int i = 0; i < extraEdgesToBeChecked.Len(); i++) {
    TPair<TInt,TInt> edge = extraEdgesToBeChecked.GetVal(i);
    TPair<TInt,TInt> edgeToBeChecked = TPair<TInt,TInt>(edge.GetVal1(),
      edges.GetDat(edge.GetVal1()).GetVal(edge.GetVal2()));
    G->IsEdge(GraphIdToNodeIdMapping.GetDat(edgeToBeChecked.GetVal1()),
              GraphIdToNodeIdMapping.GetDat(edgeToBeChecked.GetVal2()));
  }
  sw->Stop(TStopwatch::EstimateSizes);
  printf("Total time taken determining if edges are in graph: %.2f seconds\n", sw->Last(TStopwatch::EstimateSizes));

  sw->Start(TStopwatch::RemoveEdges);
  for (TVec<TInt>::TIter CurI = verticesToBeDeleted.BegI(); CurI < verticesToBeDeleted.EndI(); CurI++) {
    G->DelNode(GraphIdToNodeIdMapping.GetDat(*CurI));
  }

  for (TVec< TPair<TInt,TInt> >::TIter CurI = edgesToBeDeleted.BegI(); CurI < edgesToBeDeleted.EndI(); CurI++) {
    G->DelEdge(GraphIdToNodeIdMapping.GetDat(CurI->GetVal1()), GraphIdToNodeIdMapping.GetDat(CurI->GetVal2()));
  }
  sw->Stop(TStopwatch::RemoveEdges);
  printf("Total time taken removing edges: %.2f seconds\n\n", sw->Last(TStopwatch::RemoveEdges));

  sw->Start(TStopwatch::AddEdges);
  for (TVec<TInt>::TIter CurI = verticesToBeDeleted.BegI(); CurI < verticesToBeDeleted.EndI(); CurI++) {
    G->AddNode(GraphIdToNodeIdMapping.GetDat(*CurI));
  }

  for (TVec< TPair<TInt,TInt> >::TIter CurI = edgesToBeDeleted.BegI(); CurI < edgesToBeDeleted.EndI(); CurI++) {
    G->AddEdge(GraphIdToNodeIdMapping.GetDat(CurI->GetVal1()), GraphIdToNodeIdMapping.GetDat(CurI->GetVal2()));
  }
  sw->Stop(TStopwatch::AddEdges);
  printf("Total time taken re-adding edges: %.2f seconds\n\n", sw->Last(TStopwatch::AddEdges));
}

int main(int argc, char* argv[]) {
  THash<TInt,TInt> nodeToModeMapping = THash<TInt,TInt>();
  THash<TInt,TVec<TInt> > edges = THash<TInt,TVec<TInt> >();
  TVec<TInt> verticesToBeDeleted = TVec<TInt>();
  TVec< TPair<TInt,TInt> > edgesToBeDeleted = TVec< TPair<TInt,TInt> >();
  TVec< TPair<TInt,TInt> > verticesToBeChecked = TVec< TPair<TInt,TInt> >();
  TVec<TInt> extraVerticesToBeChecked = TVec<TInt>();
  TVec< TPair<TInt,TInt> > edgesSrcVertexToBeChecked = TVec< TPair<TInt,TInt> >();
  TVec< TPair<TInt,TInt> > edgesDstVertexToBeChecked = TVec< TPair<TInt,TInt> >();
  TVec< TPair<TInt,TInt> > extraEdgesToBeChecked = TVec< TPair<TInt,TInt> >();
  CreateGraphMapping(nodeToModeMapping, edges,
                     verticesToBeDeleted,
                     edgesToBeDeleted,
                     verticesToBeChecked,
                     extraVerticesToBeChecked,
                     edgesSrcVertexToBeChecked,
                     edgesDstVertexToBeChecked,
                     extraEdgesToBeChecked);

  testImplementationA(nodeToModeMapping, edges,
                      verticesToBeDeleted,
                      edgesToBeDeleted,
                      verticesToBeChecked,
                      extraVerticesToBeChecked,
                      edgesSrcVertexToBeChecked,
                      edgesDstVertexToBeChecked,
                      extraEdgesToBeChecked);
  testImplementationB(nodeToModeMapping, edges,
                      verticesToBeDeleted,
                      edgesToBeDeleted,
                      verticesToBeChecked,
                      extraVerticesToBeChecked,
                      edgesSrcVertexToBeChecked,
                      edgesDstVertexToBeChecked,
                      extraEdgesToBeChecked);
  testImplementationC(nodeToModeMapping, edges,
                      verticesToBeDeleted,
                      edgesToBeDeleted,
                      verticesToBeChecked,
                      extraVerticesToBeChecked,
                      edgesSrcVertexToBeChecked,
                      edgesDstVertexToBeChecked,
                      extraEdgesToBeChecked);
  testImplementationBC(nodeToModeMapping, edges,
                       verticesToBeDeleted,
                       edgesToBeDeleted,
                       verticesToBeChecked,
                       extraVerticesToBeChecked,
                       edgesSrcVertexToBeChecked,
                       edgesDstVertexToBeChecked,
                       extraEdgesToBeChecked);

  return 0;
}
