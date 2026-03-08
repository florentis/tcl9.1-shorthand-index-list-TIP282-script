*** ../../tcl9.1-shorthand-index-list-TIP282/generic/tclCompile.c	Sat Feb 28 22:11:32 2026
--- ../generic/tclCompile.c	Fri Mar  6 23:23:28 2026
*************** TclSetByteCodeFromAny(
*** 1143,1150 ****
      if (clLocPtr) {
  	compEnv.clNext = &clLocPtr->loc[0];
      }
! 
!     TclCompileScript(interp, stringPtr, length, &compEnv);
  

--- 1143,1155 ----
      if (clLocPtr) {
  	compEnv.clNext = &clLocPtr->loc[0];
      }
!     if (stringPtr[0] == '(' && stringPtr[length-1] == ')') {
! 	TclCompileExpr(interp, stringPtr, length, &compEnv, 0);
! 	TclEmitOpcode(		INST_DONE,			&compEnv);
!     } else {
! 	TclCompileScript(interp, stringPtr, length, &compEnv);
!     
  

*************** TclSetByteCodeFromAny(
*** 1170,1181 ****
--- 1175,1188 ----
  	if (clLocPtr) {
  	    compEnv.clNext = &clLocPtr->loc[0];
  	}
  	compEnv.atCmdStart = 2;		/* The disabling magic. */
  	TclCompileScript(interp, stringPtr, length, &compEnv);
  	assert (compEnv.atCmdStart > 1);
  	TclEmitOpcode(		INST_DONE,			&compEnv);
  	assert (compEnv.atCmdStart > 1);
      }
+     }
  

*************** CompileCommandTokens(
*** 2388,2422 ****
      /* Do we know the command word? */
      Tcl_IncrRefCount(cmdObj);
      tokenPtr = parsePtr->tokenPtr;
!     cmdKnown = TclWordKnownAtCompileTime(tokenPtr, cmdObj);
  
!     /* Is this a command we should (try to) compile with a compileProc ? */
!     if (cmdKnown && !(iPtr->flags & DONT_COMPILE_CMDS_INLINE)) {
! 	cmdPtr = (Command *) Tcl_GetCommandFromObj(interp, cmdObj);
! 	if (cmdPtr) {
! 	    /*
! 	     * Found a command.  Test the ways we can be told not to attempt
! 	     * to compile it.
! 	     */
! 	    if ((cmdPtr->compileProc == NULL)
  		    || (cmdPtr->nsPtr->flags & NS_SUPPRESS_COMPILATION)
  		    || (cmdPtr->flags & CMD_HAS_EXEC_TRACES)) {
! 		cmdPtr = NULL;
  	    }
! 	}
! 	if (cmdPtr && !(cmdPtr->flags & CMD_COMPILES_EXPANDED)) {
! 	    expand = ExpandRequested(parsePtr->tokenPtr, numWords);
! 	    if (expand) {
! 		/* We need to expand, but compileProc cannot. */
! 		cmdPtr = NULL;
  	    }
  	}
      }
  
      /* If cmdPtr != NULL, try to call cmdPtr->compileProc */
      if (cmdPtr) {
  	code = CompileCmdCompileProc(interp, parsePtr, cmdPtr, envPtr);
      }
  
      if (code == TCL_ERROR) {
  	/*
--- 2395,2437 ----
      /* Do we know the command word? */
      Tcl_IncrRefCount(cmdObj);
      tokenPtr = parsePtr->tokenPtr;
!     // Math mode
!     if (tokenPtr->type == TCL_TOKEN_SUB_EXPR) {
! 	TclCompileExprCmd(interp, parsePtr, NULL, envPtr);
!     } else {
! 	cmdKnown = TclWordKnownAtCompileTime(tokenPtr, cmdObj);
  
! 	/* Is this a command we should (try to) compile with a compileProc ? */
! 	if (cmdKnown && !(iPtr->flags & DONT_COMPILE_CMDS_INLINE)) {
! 	    cmdPtr = (Command *) Tcl_GetCommandFromObj(interp, cmdObj);
! 	    if (cmdPtr) {
! 		/*
! 		 * Found a command.  Test the ways we can be told not to attempt
! 		 * to compile it.
! 		 */
! 		if ((cmdPtr->compileProc == NULL)
  		    || (cmdPtr->nsPtr->flags & NS_SUPPRESS_COMPILATION)
  		    || (cmdPtr->flags & CMD_HAS_EXEC_TRACES)) {
! 		    cmdPtr = NULL;
! 		}
  	    }
! 	    if (cmdPtr && !(cmdPtr->flags & CMD_COMPILES_EXPANDED)) {
! 		expand = ExpandRequested(parsePtr->tokenPtr, numWords);
! 		if (expand) {
! 		    /* We need to expand, but compileProc cannot. */
! 		    cmdPtr = NULL;
! 		}
  	    }
  	}
      }
  
      /* If cmdPtr != NULL, try to call cmdPtr->compileProc */
      if (cmdPtr) {
  	code = CompileCmdCompileProc(interp, parsePtr, cmdPtr, envPtr);
      }
  
      if (code == TCL_ERROR) {

*************** TclCompileScript(
*** 2600,2608 ****
  	     * (considering interp recursionlimit).
  	     */
  	    iPtr->numLevels++;
! 
! 	    lastCmdIdx = CompileCommandTokens(interp, parsePtr, envPtr);
! 
  	    iPtr->numLevels--;
  
  	    /*
--- 2614,2642 ----
  	     * (considering interp recursionlimit).
  	     */
  	    iPtr->numLevels++;
! 	    
! 	    if (parsePtr->tokenPtr[0].type == TCL_TOKEN_SUB_EXPR) {
! 		//math mode
! 		Tcl_Size savedStackDepth = envPtr->currStackDepth;
! 		TclCompileExpr(interp, parsePtr->tokenPtr[1].start, parsePtr->tokenPtr[1].size, envPtr, true);
! 		
! 		 lastCmdIdx=envPtr->numCommands;
! 		 envPtr->currStackDepth = savedStackDepth;
! 		depth = TclGetStackDepth(envPtr);
! 		numBytes=0;
! 	    } else {
! 		lastCmdIdx = CompileCommandTokens(interp, parsePtr, envPtr);
! 	    }
  	    iPtr->numLevels--;
