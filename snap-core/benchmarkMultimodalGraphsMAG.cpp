// Benchmarking simple multimodal graph functionality

#include "Snap.h"

void CreateGraphMapping(THash<TStr, TInt>& nodeToModeMapping,
                        TVec< TPair<TStr,TStr> >& edges,
                        TVec<TStr>& verticesToBeDeleted,
                        TVec< TPair<TStr,TStr> >& edgesToBeDeleted,
                        TVec< TPair<TInt,TInt> >& verticesToBeChecked,
                        TVec<TStr>& extraVerticesToBeChecked,
                        TVec< TPair<TInt,TInt> >& edgesSrcVertexToBeChecked,
                        TVec< TPair<TInt,TInt> >& edgesDstVertexToBeChecked,
                        TVec<TInt>& extraEdgesToBeChecked) {
  THash<TStr, TInt> datasetIdToGraphIdMapping = THash<TStr, TInt>();
  int numModes = 0;

  printf("Reading graph from disk...\n");

  PSsParser parser = TSsParser::New("/dfs/scratch0/dataset/MicrosoftAcademicGraph/20160205/Papers.txt", ssfTabSep);
  while (parser->Next()) {
    nodeToModeMapping.AddDat(parser->GetFld(0), numModes);
    edges.Add(TPair<TStr,TStr>(parser->GetFld(0), parser->GetFld(9)));
  }
  numModes++;
  printf("Done reading Papers mode...\n");

  parser = TSsParser::New("/dfs/scratch0/dataset/MicrosoftAcademicGraph/20160205/Authors.txt", ssfTabSep);
  while (parser->Next()) {
    nodeToModeMapping.AddDat(parser->GetFld(0), numModes);
  }
  numModes++;
  printf("Done reading Authors mode...\n");

  parser = TSsParser::New("/dfs/scratch0/dataset/MicrosoftAcademicGraph/20160205/Conferences.txt", ssfTabSep);
  while (parser->Next()) {
    nodeToModeMapping.AddDat(parser->GetFld(0), numModes);
  }
  numModes++;
  printf("Done reading ConferenceSeries mode...\n");

  parser = TSsParser::New("/dfs/scratch0/dataset/MicrosoftAcademicGraph/20160205/Affiliations.txt", ssfTabSep);
  while (parser->Next()) {
    nodeToModeMapping.AddDat(parser->GetFld(0), numModes);
  }
  numModes++;
  printf("Done reading Affiliations mode...\n");

  parser = TSsParser::New("/dfs/scratch0/dataset/MicrosoftAcademicGraph/20160205/FieldsOfStudy.txt", ssfTabSep);
  while (parser->Next()) {
    nodeToModeMapping.AddDat(parser->GetFld(0), numModes);
  }
  numModes++;
  printf("Done reading FieldsOfStudy mode...\n");

  parser = TSsParser::New("/dfs/scratch0/dataset/MicrosoftAcademicGraph/20160205/PaperAuthorAffiliations.txt", ssfTabSep);
  while (parser->Next()) {
    edges.Add(TPair<TStr,TStr>(parser->GetFld(0), parser->GetFld(1)));
    edges.Add(TPair<TStr,TStr>(parser->GetFld(1), parser->GetFld(2)));
  }

  parser = TSsParser::New("/dfs/scratch0/dataset/MicrosoftAcademicGraph/20160205/PaperKeywords.txt", ssfTabSep);
  while (parser->Next()) {
    edges.Add(TPair<TStr,TStr>(parser->GetFld(0), parser->GetFld(2)));
  }

  printf("Total number of nodes: %d...\n", nodeToModeMapping.Len());
  printf("Total number of edges: %d...\n", edges.Len());

  for (int i = 0; i < 10000; i++) {
    TStr vertexToBeDeleted = nodeToModeMapping.GetKey(nodeToModeMapping.GetRndKeyId(TInt::Rnd));
    while (verticesToBeDeleted.IsIn(vertexToBeDeleted)) {
      vertexToBeDeleted = nodeToModeMapping.GetKey(nodeToModeMapping.GetRndKeyId(TInt::Rnd));
    }
    verticesToBeDeleted.Add(vertexToBeDeleted);
  }

  for (int i = 0; i < 1000000; i++) {
    TStr vertexToBeChecked = nodeToModeMapping.GetKey(nodeToModeMapping.GetRndKeyId(TInt::Rnd));
    extraVerticesToBeChecked.Add(vertexToBeChecked);
  }

  for (int i = 0; i < 1000000; i++) {
    TInt edgeIndexToBeChecked = TInt::Rnd.GetUniDevInt(edges.Len());
    extraEdgesToBeChecked.Add(edgeIndexToBeChecked);
  }

  for (int i = 0; i < 100000; i++) {
    TPair<TStr, TStr> edgeToBeDeleted = edges.GetRndVal();
    while (edgesToBeDeleted.IsIn(edgeToBeDeleted) ||
           verticesToBeDeleted.IsIn(edgeToBeDeleted.GetVal1()) ||
           verticesToBeDeleted.IsIn(edgeToBeDeleted.GetVal2())) {
      edgeToBeDeleted = edges.GetRndVal();
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

void testImplementationA(const THash<TStr,TInt>& nodeToModeMapping,
                         const TVec< TPair<TStr,TStr> >& edges,
                         const TVec<TStr>& verticesToBeDeleted,
                         const TVec< TPair<TStr,TStr> >& edgesToBeDeleted,
                         const TVec< TPair<TInt,TInt> >& verticesToBeChecked,
                         const TVec<TStr>& extraVerticesToBeChecked,
                         const TVec< TPair<TInt,TInt> >& edgesSrcVertexToBeChecked,
                         const TVec< TPair<TInt,TInt> >& edgesDstVertexToBeChecked,
                         const TVec<TInt>& extraEdgesToBeChecked) {
  PMultimodalGraphImplA G = TMultimodalGraphImplA::New();
  printf("Creating graph, implementation A...\n");
  TStopwatch* sw = TStopwatch::GetInstance();
  sw->Start(TStopwatch::LoadTables);
  THash<TStr, TPair<TInt,TInt> > GraphIdToNodeIdMapping = THash<TStr, TPair<TInt,TInt> >();
  for (THash<TStr,TInt>::TIter CurI = nodeToModeMapping.BegI(); CurI < nodeToModeMapping.EndI(); CurI++) {
    GraphIdToNodeIdMapping.AddDat(CurI.GetKey(), G->AddNode(CurI.GetDat()));
  }
  for (TVec< TPair<TStr,TStr> >::TIter CurI = edges.BegI(); CurI < edges.EndI(); CurI++) {
    G->AddEdge(GraphIdToNodeIdMapping.GetDat(CurI->GetVal1()), GraphIdToNodeIdMapping.GetDat(CurI->GetVal2()));
  }
  sw->Stop(TStopwatch::LoadTables);
  printf("Total time taken creating graph: %.2f seconds\n", sw->Last(TStopwatch::LoadTables));

  TIntV Modes1 = TIntV(); Modes1.Add(0); Modes1.Add(1);
  TIntV Modes2 = TIntV(); Modes2.Add(1); Modes2.Add(2);
  TIntV Modes3 = TIntV(); Modes3.Add(2); Modes3.Add(0);
  TIntV Modes4 = TIntV(); Modes4.Add(0); Modes4.Add(1); Modes4.Add(4);

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraph(Modes2);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 2 and 1: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraph(Modes3);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 0 and 2: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraph(Modes1);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 0 and 1: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraph(Modes4);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 0, 1 and 4: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

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
    TPair<TStr,TStr> edgeToBeChecked = edges.GetVal(extraEdgesToBeChecked.GetVal(i));
    G->IsEdge(GraphIdToNodeIdMapping.GetDat(edgeToBeChecked.GetVal1()),
              GraphIdToNodeIdMapping.GetDat(edgeToBeChecked.GetVal2()));
  }
  sw->Stop(TStopwatch::EstimateSizes);
  printf("Total time taken determining if edges are in graph: %.2f seconds\n", sw->Last(TStopwatch::EstimateSizes));

  sw->Start(TStopwatch::RemoveEdges);
  for (TVec<TStr>::TIter CurI = verticesToBeDeleted.BegI(); CurI < verticesToBeDeleted.EndI(); CurI++) {
    G->DelNode(GraphIdToNodeIdMapping.GetDat(*CurI));
  }

  for (TVec< TPair<TStr,TStr> >::TIter CurI = edgesToBeDeleted.BegI(); CurI < edgesToBeDeleted.EndI(); CurI++) {
    G->DelEdge(GraphIdToNodeIdMapping.GetDat(CurI->GetVal1()), GraphIdToNodeIdMapping.GetDat(CurI->GetVal2()));
  }
  sw->Stop(TStopwatch::RemoveEdges);
  printf("Total time taken removing edges: %.2f seconds\n\n", sw->Last(TStopwatch::RemoveEdges));
}

void testImplementationB(const THash<TStr,TInt>& nodeToModeMapping,
                         const TVec< TPair<TStr,TStr> >& edges,
                         const TVec<TStr>& verticesToBeDeleted,
                         const TVec< TPair<TStr,TStr> >& edgesToBeDeleted,
                         const TVec< TPair<TInt,TInt> >& verticesToBeChecked,
                         const TVec<TStr>& extraVerticesToBeChecked,
                         const TVec< TPair<TInt,TInt> >& edgesSrcVertexToBeChecked,
                         const TVec< TPair<TInt,TInt> >& edgesDstVertexToBeChecked,
                         const TVec<TInt>& extraEdgesToBeChecked) {
  PMultimodalGraphImplB G = TMultimodalGraphImplB::New();
  printf("Creating graph, implementation B...\n");
  TStopwatch* sw = TStopwatch::GetInstance();
  sw->Start(TStopwatch::LoadTables);
  THash<TStr, TPair<TInt,TInt> > GraphIdToNodeIdMapping = THash<TStr, TPair<TInt,TInt> >();
  for (THash<TStr,TInt>::TIter CurI = nodeToModeMapping.BegI(); CurI < nodeToModeMapping.EndI(); CurI++) {
    GraphIdToNodeIdMapping.AddDat(CurI.GetKey(), G->AddNode(CurI.GetDat()));
  }
  for (TVec< TPair<TStr,TStr> >::TIter CurI = edges.BegI(); CurI < edges.EndI(); CurI++) {
    G->AddEdge(GraphIdToNodeIdMapping.GetDat(CurI->GetVal1()), GraphIdToNodeIdMapping.GetDat(CurI->GetVal2()));
  }
  sw->Stop(TStopwatch::LoadTables);
  printf("Total time taken creating graph: %.2f seconds\n", sw->Last(TStopwatch::LoadTables));

  TIntV Modes1 = TIntV(); Modes1.Add(0); Modes1.Add(1);
  TIntV Modes2 = TIntV(); Modes2.Add(1); Modes2.Add(2);
  TIntV Modes3 = TIntV(); Modes3.Add(2); Modes3.Add(0);
  TIntV Modes4 = TIntV(); Modes4.Add(0); Modes4.Add(1); Modes4.Add(4);

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraph(Modes2);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 2 and 1: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraph(Modes3);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 0 and 2: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraph(Modes1);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 0 and 1: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraph(Modes4);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 0, 1 and 4: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

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
    TPair<TStr,TStr> edgeToBeChecked = edges.GetVal(extraEdgesToBeChecked.GetVal(i));
    G->IsEdge(GraphIdToNodeIdMapping.GetDat(edgeToBeChecked.GetVal1()),
              GraphIdToNodeIdMapping.GetDat(edgeToBeChecked.GetVal2()));
  }
  sw->Stop(TStopwatch::EstimateSizes);
  printf("Total time taken determining if edges are in graph: %.2f seconds\n", sw->Last(TStopwatch::EstimateSizes));

  sw->Start(TStopwatch::RemoveEdges);
  for (TVec<TStr>::TIter CurI = verticesToBeDeleted.BegI(); CurI < verticesToBeDeleted.EndI(); CurI++) {
    G->DelNode(GraphIdToNodeIdMapping.GetDat(*CurI));
  }

  for (TVec< TPair<TStr,TStr> >::TIter CurI = edgesToBeDeleted.BegI(); CurI < edgesToBeDeleted.EndI(); CurI++) {
    G->DelEdge(GraphIdToNodeIdMapping.GetDat(CurI->GetVal1()), GraphIdToNodeIdMapping.GetDat(CurI->GetVal2()));
  }
  sw->Stop(TStopwatch::RemoveEdges);
  printf("Total time taken removing edges: %.2f seconds\n\n", sw->Last(TStopwatch::RemoveEdges));
}

void testImplementationC(const THash<TStr,TInt>& nodeToModeMapping,
                         const TVec< TPair<TStr,TStr> >& edges,
                         const TVec<TStr>& verticesToBeDeleted,
                         const TVec< TPair<TStr,TStr> >& edgesToBeDeleted,
                         const TVec< TPair<TInt,TInt> >& verticesToBeChecked,
                         const TVec<TStr>& extraVerticesToBeChecked,
                         const TVec< TPair<TInt,TInt> >& edgesSrcVertexToBeChecked,
                         const TVec< TPair<TInt,TInt> >& edgesDstVertexToBeChecked,
                         const TVec<TInt>& extraEdgesToBeChecked) {
  PMultimodalGraphImplC G = TMultimodalGraphImplC::New();
  printf("Creating graph, implementation C...\n");
  TStopwatch* sw = TStopwatch::GetInstance();
  sw->Start(TStopwatch::LoadTables);
  THash<TStr, TPair<TInt,TInt> > GraphIdToNodeIdMapping = THash<TStr, TPair<TInt,TInt> >();
  for (THash<TStr,TInt>::TIter CurI = nodeToModeMapping.BegI(); CurI < nodeToModeMapping.EndI(); CurI++) {
    GraphIdToNodeIdMapping.AddDat(CurI.GetKey(), G->AddNode(CurI.GetDat()));
  }
  for (TVec< TPair<TStr,TStr> >::TIter CurI = edges.BegI(); CurI < edges.EndI(); CurI++) {
    G->AddEdge(GraphIdToNodeIdMapping.GetDat(CurI->GetVal1()), GraphIdToNodeIdMapping.GetDat(CurI->GetVal2()));
  }
  sw->Stop(TStopwatch::LoadTables);
  printf("Total time taken creating graph: %.2f seconds\n", sw->Last(TStopwatch::LoadTables));

  TIntV Modes1 = TIntV(); Modes1.Add(0); Modes1.Add(1);
  TIntV Modes2 = TIntV(); Modes2.Add(1); Modes2.Add(2);
  TIntV Modes3 = TIntV(); Modes3.Add(2); Modes3.Add(0);
  TIntV Modes4 = TIntV(); Modes4.Add(0); Modes4.Add(1); Modes4.Add(4);

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraph(Modes2);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 2 and 1: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraph(Modes3);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 0 and 2: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraph(Modes1);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 0 and 1: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraph(Modes4);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 0, 1 and 4: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

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
    TPair<TStr,TStr> edgeToBeChecked = edges.GetVal(extraEdgesToBeChecked.GetVal(i));
    G->IsEdge(GraphIdToNodeIdMapping.GetDat(edgeToBeChecked.GetVal1()),
              GraphIdToNodeIdMapping.GetDat(edgeToBeChecked.GetVal2()));
  }
  sw->Stop(TStopwatch::EstimateSizes);
  printf("Total time taken determining if edges are in graph: %.2f seconds\n", sw->Last(TStopwatch::EstimateSizes));

  sw->Start(TStopwatch::RemoveEdges);
  for (TVec<TStr>::TIter CurI = verticesToBeDeleted.BegI(); CurI < verticesToBeDeleted.EndI(); CurI++) {
    G->DelNode(GraphIdToNodeIdMapping.GetDat(*CurI));
  }

  for (TVec< TPair<TStr,TStr> >::TIter CurI = edgesToBeDeleted.BegI(); CurI < edgesToBeDeleted.EndI(); CurI++) {
    G->DelEdge(GraphIdToNodeIdMapping.GetDat(CurI->GetVal1()), GraphIdToNodeIdMapping.GetDat(CurI->GetVal2()));
  }
  sw->Stop(TStopwatch::RemoveEdges);
  printf("Total time taken removing edges: %.2f seconds\n\n", sw->Last(TStopwatch::RemoveEdges));
}

void testImplementationBC(const THash<TStr,TInt>& nodeToModeMapping,
                          const TVec< TPair<TStr,TStr> >& edges,
                          const TVec<TStr>& verticesToBeDeleted,
                          const TVec< TPair<TStr,TStr> >& edgesToBeDeleted,
                          const TVec< TPair<TInt,TInt> >& verticesToBeChecked,
                          const TVec<TStr>& extraVerticesToBeChecked,
                          const TVec< TPair<TInt,TInt> >& edgesSrcVertexToBeChecked,
                          const TVec< TPair<TInt,TInt> >& edgesDstVertexToBeChecked,
                          const TVec<TInt>& extraEdgesToBeChecked) {
  PMultimodalGraphImplBC G = TMultimodalGraphImplBC::New();
  printf("Creating graph, implementation BC...\n");
  TStopwatch* sw = TStopwatch::GetInstance();
  sw->Start(TStopwatch::LoadTables);
  THash<TStr, TPair<TInt,TInt> > GraphIdToNodeIdMapping = THash<TStr, TPair<TInt,TInt> >();
  for (THash<TStr,TInt>::TIter CurI = nodeToModeMapping.BegI(); CurI < nodeToModeMapping.EndI(); CurI++) {
    GraphIdToNodeIdMapping.AddDat(CurI.GetKey(), G->AddNode(CurI.GetDat()));
  }
  for (TVec< TPair<TStr,TStr> >::TIter CurI = edges.BegI(); CurI < edges.EndI(); CurI++) {
    G->AddEdge(GraphIdToNodeIdMapping.GetDat(CurI->GetVal1()), GraphIdToNodeIdMapping.GetDat(CurI->GetVal2()));
  }
  sw->Stop(TStopwatch::LoadTables);
  printf("Total time taken creating graph: %.2f seconds\n", sw->Last(TStopwatch::LoadTables));

  TIntV Modes1 = TIntV(); Modes1.Add(0); Modes1.Add(1);
  TIntV Modes2 = TIntV(); Modes2.Add(1); Modes2.Add(2);
  TIntV Modes3 = TIntV(); Modes3.Add(2); Modes3.Add(0);
  TIntV Modes4 = TIntV(); Modes4.Add(0); Modes4.Add(1); Modes4.Add(4);

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraph(Modes2);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 2 and 1: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraph(Modes3);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 0 and 2: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraph(Modes1);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 0 and 1: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

  sw->Start(TStopwatch::BuildSubgraph);
  G->GetSubGraph(Modes4);
  sw->Stop(TStopwatch::BuildSubgraph);
  printf("Total time taken creating sub-graph between modes 0, 1 and 4: %.2f seconds\n", sw->Last(TStopwatch::BuildSubgraph));

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
    TPair<TStr,TStr> edgeToBeChecked = edges.GetVal(extraEdgesToBeChecked.GetVal(i));
    G->IsEdge(GraphIdToNodeIdMapping.GetDat(edgeToBeChecked.GetVal1()),
              GraphIdToNodeIdMapping.GetDat(edgeToBeChecked.GetVal2()));
  }
  sw->Stop(TStopwatch::EstimateSizes);
  printf("Total time taken determining if edges are in graph: %.2f seconds\n", sw->Last(TStopwatch::EstimateSizes));

  sw->Start(TStopwatch::RemoveEdges);
  for (TVec<TStr>::TIter CurI = verticesToBeDeleted.BegI(); CurI < verticesToBeDeleted.EndI(); CurI++) {
    G->DelNode(GraphIdToNodeIdMapping.GetDat(*CurI));
  }

  for (TVec< TPair<TStr,TStr> >::TIter CurI = edgesToBeDeleted.BegI(); CurI < edgesToBeDeleted.EndI(); CurI++) {
    G->DelEdge(GraphIdToNodeIdMapping.GetDat(CurI->GetVal1()), GraphIdToNodeIdMapping.GetDat(CurI->GetVal2()));
  }
  sw->Stop(TStopwatch::RemoveEdges);
  printf("Total time taken removing edges: %.2f seconds\n\n", sw->Last(TStopwatch::RemoveEdges));
}

int main(int argc, char* argv[]) {
  THash<TStr,TInt> nodeToModeMapping = THash<TStr,TInt>();
  TVec< TPair<TStr,TStr> > edges = TVec< TPair<TStr,TStr> >();
  TVec<TStr> verticesToBeDeleted = TVec<TStr>();
  TVec< TPair<TStr,TStr> > edgesToBeDeleted = TVec< TPair<TStr,TStr> >();
  TVec< TPair<TInt,TInt> > verticesToBeChecked = TVec< TPair<TInt,TInt> >();
  TVec<TStr> extraVerticesToBeChecked = TVec<TStr>();
  TVec< TPair<TInt,TInt> > edgesSrcVertexToBeChecked = TVec< TPair<TInt,TInt> >();
  TVec< TPair<TInt,TInt> > edgesDstVertexToBeChecked = TVec< TPair<TInt,TInt> >();
  TVec<TInt> extraEdgesToBeChecked = TVec<TInt>();
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
