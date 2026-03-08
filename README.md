# Expr shorthand with « script option » #

## Principle : ##

> if a script begins with an open-parenthese and finishes with a closed-parenthese, then it wil be compiled as an expression.

It includes the other *expr shorthand changes*. Summery :

  | Designation | Syntax allowed | Effect |
  | :---        |:----   |:--- |
  | Inline shorthand expr substitution | `[(*expression*)]` | Substitutes *expression* by its value |
  | Var index shorthand substitution | `set A((*expression*)) $val` | Sets array at a key which is evaluated as an *expression* |
  | Native list handling | `expr {(*expression*, *expression*, ...)}` | Returns a list of evaluated expressions |
  | TIP282 integration (1) : allow many instructions | `expr {*expression1*; *expression2*}` | Evaluates many expressions, returns the last one |
  | TIP282 integration (2) : assignement operator `=`| `expr {i = j = k = $value}` | Sets *vars* to $value|
  | Scripted expression (add by this repository) | `{(....)}` | Every script begining with a `(` and closed by a `)` is evaluated as an *expression*|

It should work with any command which compiles its arguments : `if`, `while`, `for`, `foreach`, `lmap`, `eval`, `apply`, `proc`,...etc

## Examples ##

```tcl
for {(i=0)} {$i < 10} {(i=$i+1)} {puts $i}

eval {(1+3)}

set add {{x y} {($x+$y)}
puts [apply $add 12 32]; # 44

# complex lassign (3x3 matrix) with lmap (see above)
set varMAP {{a b c} {d e f} {g h i}}
lmap row1 $varMAP row2 $M {
	     lmap var $row1 val $row2 {($var = double($val))}
}

# Transpose : transpose a matrix :
# using :
# - script expression in lmap
# - TIP282 assignement
# - inline expression shorthand
# - Native list handling
proc transpose {M} {
    set MAP {{a b c} {d e f} {g h i}}
    lmap row1 $MAP row2 $M {
	     lmap e1 $row1 e2 $row2 {($e1 = double($e2))}
    }
    return [( ($a, $d, $g),
	            ($b, $e, $h),
	            ($c, $f, $i) )]
}

# dim : Give the dimension of a nested list
# using :
# - script expression in foreach
# - TIP282 assignement
proc dim { {T \  } {depth 0}} {
   set len [llength $T]
   set L [lmap e $T {llength $e}]
   if {[llength [lsearch -all $L 1]] == $len} {
	    return $len
   } elseif {[llength [lsearch -all $L 1]] == 0} {
	    set sort [lsort $L]
	    if {[lindex $sort 0] != [lindex $sort end]} {
	       error "tenseur inhomogene"
      } else {
	       foreach e $T {(
		        inf=[dim $e [incr depth]]
         )}
	       return [list $len {*}$inf]
      }
   } else {
	    error "tenseur inhomogene at depth : $depth, index [lsearch -not -all $L 1]"
   }
}

# comatrix : give the comatrix of a matrix
# using :
# - script expression in lmap
# - TIP282 assignement
# - inline expression shorthand
# - Native list handling

proc comatrix {M} {
   set MAP {{a b c} {d e f} {g h i}}
   lmap row1 $MAP row2 $M {
	    lmap e1 $row1 e2 $row2 {($e1 = double($e2))}
   }
   return [( ($e*$i - $f*$h, $f*$g - $d*$i, $d*$h - $e*$g),
	           ($c*$h - $b*$i, $a*$i - $c*$g, $b*$g - $a*$h),
	           ($b*$f - $c*$e, $c*$d - $a*$f, $a*$e - $b*$d) )]
}

```
