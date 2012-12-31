shiba
=====

Simulated historical island biogeography analysis - Cam Webb (after discussions with Rick Ree, Michael Donoghue and Brian Moore)

Very brief info!

Lineage of 6 extant spp evolving over 60 my in two areas

Move through each time slice, check for movement, fossil pass-through, check for local extinction, lineage extinction/speciation.  Keep the ones that reach the extant distribution.

Do once for a single start location, once for a widespread start location.


 for the simple 3 taxa case - to get a winning set, set all extant only to borneo, with psurv 0.7 pdisp 0.3 and sdisp -0.003

----

dispersal model 

-----

Distant runs:

bash $ nohup ./shiba -s > output 2> /dev/null &
tcsh % nohup ( shiba -s > output ) >& /dev/null &


----

## Infile.xml

The normative schema is `infile.rnc`: `infile.rng` is derived
