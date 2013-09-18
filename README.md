Welcome to the New HZG Analyzer, by Brian Pollack
=================================================

This branch is in production, intended to run on the bleeding edge NWU ntuples.
Significant updates are intended for readability, speed improvements, and workflow optimization.

Notes:
------
  * Be careful of vector<ROOTstuff>, especially TCPhoton, vectors do not clean properly
  * Valgrind claims TCPhoton has mismatched delete in destructor, I think this is a ROOT issue, not sure how to fix
  * There is now a segfault on termination of ROOT, debugging has not shown the cause, most likely some corrupted memory somewhere along the line

