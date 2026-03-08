*************** TclNREvalObjEx(
*** 6291,6297 ****
--- 6294,6306 ----
  	Tcl_IncrRefCount(objPtr);
  
  	script = TclGetStringFromObj(objPtr, &numSrcBytes);
+         
+         if (script [0] ==  '('  && script[numSrcBytes-1] == ')') {
+             Tcl_Obj * newExprObj;
+             result = Tcl_ExprObj(interp, objPtr, &newExprObj);
+         } else {
  	result = Tcl_EvalEx(interp, script, numSrcBytes, flags);
+         }
  

  
