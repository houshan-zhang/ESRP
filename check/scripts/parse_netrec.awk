/Finish./ {
   abort = 0;
}
/\(a\) Optimal objective value:/ {
   pb = $5;
}
/\(b\) Relaxed objective value:/ {
   db = $5;
}
/\(c\) Number of branch-and-bound nodes:/ {
   bbnodes = $8;
}
/\(e\) Total time:/ {
   time = $4;
}
/^Number of Customers:/ {
   ncust = $4;
}
/^Number of Suppliers:/ {
   nsupp = $4;
}
