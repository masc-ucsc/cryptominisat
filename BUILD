#  This file is distributed under the BSD 3-Clause License. See LICENSE for details.

cc_library(
   name = "cryptominisat",
   copts = ["-w","-O2"],  # Always fast, no warnings to avoid spurious messages
   srcs = glob(["cryptominisat5/*.cpp","cryptominisat5/*.c","genfiles/*.cpp"],exclude=["cryptominisat5/toplevelgauss.cpp","cryptominisat5/main*.cpp","cryptominisat5/watcharray_*.cpp","cryptominisat5/fuzz*.cpp","cryptominisat5/sqlitestats.cpp","cryptominisat5/gatefinder.cpp"]),
   hdrs = glob(["cryptominisat5/*.h","include/**/*.h"])
   ,
   linkopts = ["-lpthread"],
   includes = ["src","include","genfiles"],
   visibility = ["//visibility:public"],
   defines = ["USE_GAUSS"],
)

