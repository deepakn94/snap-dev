import subprocess

def produceStats(filenames):
  stats = dict()
  impls = list()
  for filename in filenames:
    with open(filename, 'r') as f:
      lastImpl = None
      for line in f:
        if "implementation" in line:
          currentImpl = line.split("implementation")[1].strip().strip(".") + \
            "--%s" % (filename.split("/")[1].split(".txt")[0])
        elif "Total time" in line:
          timingDesc = line.split(":")[0].split("Total time taken")[1].strip()
          timingDesc = timingDesc.replace("creating sub-graph between modes ", "SG(")
          timingDesc = timingDesc.replace("getting node ids in modes ", "GNIds(") 
          if "SG(" in timingDesc or "GNIds(" in timingDesc: timingDesc += ")"
          timingDesc = timingDesc.replace(" and", ",")
          timingDesc = timingDesc.replace("creating graph", "CG")
          timingDesc = timingDesc.replace("determining if nodes are not in graph", "IsNodeNot")
          timingDesc = timingDesc.replace("determining if nodes are in graph", "IsNode")
          timingDesc = timingDesc.replace("determining if edges are not in graph", "IsEdgeNot")
          timingDesc = timingDesc.replace("determining if edges are in graph", "IsEdge")
          timingDesc = timingDesc.replace("removing edges", "EdgeRemove")
          timingDesc = timingDesc.replace("re-adding edges", "EdgeReinsert")
          timingDesc = timingDesc.replace("doing one-hop BFS traversal from an arbitray set of starting nodes", "BFSOneHop")
          timingDesc = timingDesc.replace(", ", ",")  

          time = float(line.split(":")[1].split("seconds")[0].strip())
          if timingDesc not in stats:
            stats[timingDesc] = dict()
          stats[timingDesc][currentImpl] = time
          if currentImpl is not None: impls.append(currentImpl)
  impls = list(set(impls))
  return stats, impls

def produceGraphsByWorkload(stats, impls, outputFileTemplate, rowsToPlot, colsToPlot):
  tempDataFile = "tmp.dat"
  xtics = list()
  xticsIdx = 1
  with open(tempDataFile, 'w') as f:
    for timingDesc in rowsToPlot:
      f.write(str(xticsIdx))
      xtics.append("\"%s\" %d" % (timingDesc, xticsIdx))
      for impl in colsToPlot:
        if impl in stats[timingDesc]:
          f.write("\t" + str(stats[timingDesc][impl]))
        else:
          f.write("\t0.0")
      f.write("\n")
      xticsIdx += 1
  titles = colsToPlot
  plotGraph("Workloads", "Time (in seconds)", outputFileTemplate, xtics, titles, tempDataFile)

def produceGraphsByBenchmark(stats, impls, outputFileTemplate, rowsToPlot, colsToPlot, timingDesc):
  tempDataFile = "tmp.dat"
  xtics = list()
  xticsIdx = 1
  with open(tempDataFile, 'w') as f:
    for benchmark in rowsToPlot:
      f.write(str(xticsIdx))
      xtics.append("\"%s\" %d" % (benchmark, xticsIdx))
      for impl in colsToPlot:
        impl = impl + "--" + benchmark
        if timingDesc in stats and impl in stats[timingDesc]:
          f.write("\t" + str(stats[timingDesc][impl]))
        else:
          f.write("\t0.0")
      f.write("\n")
      xticsIdx += 1
  titles = colsToPlot
  plotGraph("Graph type", "Time (in seconds)", outputFileTemplate, xtics, titles, tempDataFile, True)

def plotGraph(xLabel, yLabel, outputFileTemplate, xtics, titles, tempDataFile, linesPoints=False):
  plots = []
  j = 2
  for title in titles:
    title = title.replace("_", "\_")
    lineType = "points"
    if linesPoints: lineType = "linespoints"
    plots.append(
      "\"%s\" using 1:%d with %s ls %d tit '%s'" % (
        tempDataFile, j, lineType, j, title))
    j += 1

  outputFile = outputFileTemplate + ".eps"
  gnuplotStr = """set terminal postscript eps color
  set size 0.65
  set output '%s'
  set xlabel "%s"
  set ylabel "%s"
  set xtics (%s)
  set logscale y
  set xrange[0:%d]
  set yrange[0.001:]

  set xtics border nomirror
  set ytics border nomirror
  set xtics font "Arial,8"
  set style fill empty
  set grid

  set key under
  set key spacing 2

  set style line 1 lw 2 lt 1 lc rgb 'red' pt 7 ps 1;
  set style line 2 lw 1 lt 9 lc rgb 'dark-green' pt 13 ps 1;
  set style line 3 lw 1 lt 2 lc rgb 'blue' pt 9 ps 1;
  set style line 4 lw 1 lt 4 lc rgb 'black' pt 11 ps 1;
  set style line 5 lw 1 lt 6 lc rgb 'orange' pt 1 ps 1;
  set style line 6 lw 1 lt 5 lc rgb 'coral' pt 5 ps 1;
  set style line 7 lw 1 lt 8 lc rgb 'violet' pt 3 ps 1;

  plot %s
  """ % (outputFile, xLabel, yLabel, ", ".join(xtics), len(xtics)+1, ", ".join(plots))

  with open("plot.gnuplot", "w") as f:
    f.write(gnuplotStr)
  subprocess.call("gnuplot plot.gnuplot", shell=True)
  subprocess.call("epstopdf %s" % outputFile, shell=True)
  subprocess.call("rm %s" % outputFile, shell=True)
  # subprocess.call("rm %s" % tempDataFile, shell=True)
  subprocess.call("rm plot.gnuplot", shell=True)
    

if __name__ == '__main__':
  stats, impls = produceStats(["part3/benchmark1Part2.txt", "part3/benchmark2Part2.txt", "part3/benchmark3.txt",
                               "part3/benchmark4.txt", "part3/benchmark5.txt", "part3/benchmark6Part2.txt",
                               "part3/benchmark7.txt", "part3/benchmark8.txt", "part3/benchmark9.txt",
                               "part3/benchmark10.txt", "part3/benchmark11.txt", "part3/benchmark12.txt",
                               "part3/benchmark13.txt", "part3/benchmark14.txt"])
  produceGraphsByWorkload(stats, impls, "sensitivity", ["SG(0,1)", "SG(0,1,4)", "SG(0 to 9)", "GNIds(0,1,3)"],
                          ["A--benchmark8", "A--benchmark9", "A--benchmark10", "A--benchmark11"])
  produceGraphsByWorkload(stats, impls, "implementations",
                          ["SG(0,1)", "SG(0,1,4)", "SG(0 to 9)", "GNIds(0,1,3)", "BFSOneHop"],
                          ["A--benchmark6Part2", "B--benchmark6Part2", "C--benchmark6Part2", "BC--benchmark6Part2"])
  produceGraphsByBenchmark(stats, impls, "degreeDistributionsSG01",
                           ["benchmark12", "benchmark13", "benchmark14"],
                           ["A", "B", "C", "BC"], "SG(0,1)")
  produceGraphsByBenchmark(stats, impls, "degreeDistributionsSG014",
                           ["benchmark12", "benchmark13", "benchmark14"],
                           ["A", "B", "C", "BC"], "SG(0,1,4)")
  produceGraphsByBenchmark(stats, impls, "degreeDistributionsSG0to9",
                           ["benchmark12", "benchmark13", "benchmark14"],
                           ["A", "B", "C", "BC"], "SG(0 to 9)")
  produceGraphsByBenchmark(stats, impls, "degreeDistributionsBFSOneHop",
                           ["benchmark12", "benchmark13", "benchmark14"],
                           ["A", "B", "C", "BC"], "BFSOneHop")
