# status
# 1) abort              program interrupt
# 2) timlim             time limit reached
# 3) memlim             memory limit reached
# 4) ok                 solver thinks the minimum gap has been reached

BEGIN {
   infty = +1e+20;
   eps = 1e-04;
   largegap = 1e+04;
   reltol = 1e-06;

   # initialize summary data
   ninstance = 0;
   ntimlim = 0;
   nmemlim = 0;
   nfailed = 0;

   # init avetime and avenode
   shifttime = 1;
   shiftnodes = 1;
   geoavetime = 1;
   geoavenodes = 1;

   # print title
   printf("----------------------------+------------+------------+--------------+--------------+-------+---------+--------+--------\n");
   printf("Name                        | NCustomers | NSuppliers |  Dual Bound  | Primal Bound |  Gap%% |  Nodes  |  Time  | Status \n");
   printf("----------------------------+------------+------------+--------------+--------------+-------+---------+--------+--------\n");
}

function abs(x)
{
   return x < 0 ? -x : x;
}
function min(x,y)
{
   return (x) < (y) ? (x) : (y);
}
function max(x,y)
{
   return (x) > (y) ? (x) : (y);
}

# name of each instance
/^@01/ {
   m = split($3, b, ",");
   prob = b[1];
   seed = $5

   # initialize data to be set in parse.awk of each instance
   maxtim = 0;
   maxgap = 0;
   time = 0.0;

   # initialize data to be set parse_<solver>.awk of each instance
   bbnodes = 0;
   pb = +infty;
   db = -infty;
   abort = 1;
   timlim = 0;
   memlim = 0;
}
# time
/@04/ { maxtim = $3; }
# gap
/@06/ {
   maxgap = $3;
   if( maxgap > 0 )
      reltol = maxgap;
}
/^= over =/ {
   # determine solving status
   if( time >= maxtim )
      timlim = 1;
   if( abort )
      status = "abort";
   else if( timlim )
      status = "timlim";
   else if( memlim )
      status = "memlim";
   else
      status = "ok";

   # determine overall status from solving status and solution status:
   # instance solved correctly (including case that no solution was found)
   if( status == "ok" )
   {
      geoavetime = (time+shifttime)^(1/(ninstance+1)) * geoavetime^( (ninstance)/(ninstance+1));
      geoavenodes = (bbnodes+shiftnodes)^(1/(ninstance+1)) * geoavenodes^( (ninstance)/(ninstance+1));
      nsolved++;
   }
   # incorrect solving process or infeasible solution (including errors with solution checker)
   else if( status == "abort" )
   {
      geoavetime = (maxtim+shifttime)^(1/(ninstance+1)) * geoavetime^( (ninstance)/(ninstance+1));
      geoavenodes = (bbnodes+shiftnodes)^(1/(ninstance+1)) * geoavenodes^( (ninstance)/(ninstance+1));
      nfailed++;
   }
   # stopped due to imposed limits
   else if( status == "memlim" )
   {
      geoavetime = (maxtim+shifttime)^(1/(ninstance+1)) * geoavetime^( (ninstance)/(ninstance+1));
      geoavenodes = (bbnodes+shiftnodes)^(1/(ninstance+1)) * geoavenodes^( (ninstance)/(ninstance+1));
      nmemlim++;
   }
   else
   {
      geoavetime = (maxtim+shifttime)^(1/(ninstance+1)) * geoavetime^( (ninstance)/(ninstance+1));
      geoavenodes = (bbnodes+shiftnodes)^(1/(ninstance+1)) * geoavenodes^( (ninstance)/(ninstance+1));
      ntimlim++;
   }
   ninstance++;

   # compute gap
   temp = pb;
   pb = 1.0*temp;
   temp = db;
   db = 1.0*temp;

   # we follow the gap definition in article "Progress in Presolving for Mixed Integer Programming"
   if( abs(pb - db) < eps && pb < +infty )
      gap = 0.0;
   else if( abs(db) < eps || abs(pb) < eps)
      gap = -1.0;
   else if( pb*db < 0.0 )
      gap = -1.0;
   else if( abs(db) >= +infty || abs(pb) >= +infty )
      gap = -1.0;
   else
      gap = 100.0*abs((pb-db)/max(abs(db), abs(pb)));

   if( gap < 0.0 )
      gapstr = "    --";
   else if( gap < largegap )
      gapstr = sprintf("%6.2f", gap);
   else
      gapstr = " Large";
      printf("%-28s %12d %12d %14.8g %14.8g %7s %9d %8.2f %8s\n", prob "[" seed "]", ncust, nsupp, db, pb, gapstr, bbnodes, time, status);
}
END {
   printf("----------------------------+------------+------------+--------------+--------------+-------+---------+--------+--------\n");
   printf("nsolved/ntimlim/nfailed/nmemlim: %d/%d/%d/%d\n", nsolved, ntimlim, nfailed, nmemlim);
   printf("geotime: %f\n", geoavetime-shifttime);
   printf("geonodes: %f\n", geoavenodes-shiftnodes);
   printf("\n");
}
