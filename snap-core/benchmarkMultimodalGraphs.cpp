// Benchmarking simple multimodal graph functionality

#include "Snap.h"

void CreateGraphMapping(THash<TStr, TInt>& nodeToModeMapping,
                        TVec< TPair<TStr, TStr> >& edges,
                        TVec<TStr>& verticesToBeDeleted,
                        TVec< TPair<TStr, TStr> >& edgesToBeDeleted) {
  THash<TStr, TInt> datasetIdToGraphIdMapping = THash<TStr, TInt>();

  PSsParser parser = TSsParser::New("/dfs/ilfs2/0/multimodal/FlickrTables/photosNUS/photos.tsv", ssfTabSep);
  while (parser->Next()) {
    nodeToModeMapping.AddDat(parser->GetFld(0), 0);
  }

  parser = TSsParser::New("/dfs/ilfs2/0/multimodal/FlickrTables/photosNUS/users.tsv", ssfTabSep);
  while (parser->Next()) {
    nodeToModeMapping.AddDat(parser->GetFld(0), 1);
  }

  parser = TSsParser::New("/dfs/ilfs2/0/multimodal/FlickrTables/photosNUS/comments.tsv", ssfTabSep);
  while (parser->Next()) {
    nodeToModeMapping.AddDat(parser->GetFld(0), 2);
  }

  parser = TSsParser::New("/dfs/ilfs2/0/multimodal/FlickrTables/photosNUS/locations.tsv", ssfTabSep);
  while (parser->Next()) {
    nodeToModeMapping.AddDat(parser->GetFld(0), 3);
  }

  parser = TSsParser::New("/dfs/ilfs2/0/multimodal/FlickrTables/photosNUS/tags.tsv", ssfTabSep);
  while (parser->Next()) {
    nodeToModeMapping.AddDat(parser->GetFld(0), 4);
  }

  parser = TSsParser::New("/dfs/ilfs2/0/multimodal/FlickrTables/photosNUS/photo_comment_edges.tsv", ssfTabSep);
  while (parser->Next()) {
    edges.Add(TPair<TStr,TStr>(parser->GetFld(0), parser->GetFld(1)));
  }

  parser = TSsParser::New("/dfs/ilfs2/0/multimodal/FlickrTables/photosNUS/photo_owner_edges.tsv", ssfTabSep);
  while (parser->Next()) {
    edges.Add(TPair<TStr,TStr>(parser->GetFld(0), parser->GetFld(1)));
  }

  parser = TSsParser::New("/dfs/ilfs2/0/multimodal/FlickrTables/photosNUS/comment_user_edges.tsv", ssfTabSep);
  while (parser->Next()) {
    edges.Add(TPair<TStr,TStr>(parser->GetFld(0), parser->GetFld(1)));
  }

  parser = TSsParser::New("/dfs/ilfs2/0/multimodal/FlickrTables/photosNUS/photo_location_edges.tsv", ssfTabSep);
  while (parser->Next()) {
    edges.Add(TPair<TStr,TStr>(parser->GetFld(0), parser->GetFld(1)));
  }

  parser = TSsParser::New("/dfs/ilfs2/0/multimodal/FlickrTables/photosNUS/photo_tag_edges.tsv", ssfTabSep);
  while (parser->Next()) {
    edges.Add(TPair<TStr,TStr>(parser->GetFld(0), parser->GetFld(1)));
  }

  parser = TSsParser::New("/dfs/ilfs2/0/multimodal/FlickrTables/photosNUS/tagger_tag_edges.tsv", ssfTabSep);
  while (parser->Next()) {
    edges.Add(TPair<TStr,TStr>(parser->GetFld(0), parser->GetFld(1)));
  }

  edges.Shuffle(TInt::Rnd);

  printf("Total number of nodes: %d...\n", nodeToModeMapping.Len());
  printf("Total number of edges: %d...\n", edges.Len());

  for (int i = 0; i < 10000; i++) {
    TStr vertexToBeDeleted = nodeToModeMapping.GetKey(nodeToModeMapping.GetRndKeyId(TInt::Rnd));
    while (verticesToBeDeleted.IsIn(vertexToBeDeleted)) {
      vertexToBeDeleted = nodeToModeMapping.GetKey(nodeToModeMapping.GetRndKeyId(TInt::Rnd));
    }
    verticesToBeDeleted.Add(vertexToBeDeleted);
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
}

void testImplementationA(const THash<TStr,TInt>& nodeToModeMapping,
                         const TVec< TPair<TStr,TStr> >& edges,
                         const TVec<TStr>& verticesToBeDeleted,
                         const TVec< TPair<TStr,TStr> >& edgesToBeDeleted) {
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
                         const TVec< TPair<TStr,TStr> >& edgesToBeDeleted) {
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
                         const TVec< TPair<TStr,TStr> >& edgesToBeDeleted) {
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
  CreateGraphMapping(nodeToModeMapping, edges, verticesToBeDeleted, edgesToBeDeleted);

  testImplementationA(nodeToModeMapping, edges, verticesToBeDeleted, edgesToBeDeleted);
  testImplementationB(nodeToModeMapping, edges, verticesToBeDeleted, edgesToBeDeleted);
  testImplementationC(nodeToModeMapping, edges, verticesToBeDeleted, edgesToBeDeleted);

  return 0;
}
