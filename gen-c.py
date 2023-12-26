import sys
import datetime

year = datetime.datetime.now().year
module = f"""#include "c.h"

// MACROS

// TYPES

// PROTOTYPES

// DATA

// FUNCTIONS
"""

module_begin_path = "./src/"

header = """
// MACROS

// TYPEDEFS

#include "config.h"

// INTERFACES (page 78)

// TYPES

// DATA

// FUNCTIONS
"""

header_begin_path = "./include/"

# accepts
# -h or -m (header or module)
# -f <output file>
if __name__ == "__main__":
    sys_args = sys.argv[1:]
    if len(sys_args) < 1:
        print("No arguments given")
        sys.exit(1)

    type_ = 0
    output = None

    for i, arg in enumerate(sys_args):
        match arg:
            case "-h":
                type_ = 1
            case "-m":
                type_ |= 2
            case "-f":
                # get the next argument if there is one
                if i + 1 < len(sys_args):
                    output = sys_args[i + 1]
                else:
                    print("No output file given")
                    sys.exit(1)


    if type_ == 0:
        print("No type given")
        sys.exit(1)
    elif type_ == 1:
        if output is None:
            print("No output file given")
            sys.exit(1)
        else:
            with open(header_begin_path + output + '.h', "w") as f:
                f.write(header)
    elif type_ == 2:
        if output is None:
            print("No output file given")
            sys.exit(1)
        else:
            with open(module_begin_path + output + '.c', "w") as f:
                f.write(module)



# EXPECTED FILES
# ch 1 = c.h
# ch 2, 3, 4 = alloc.c string.c sym.c types.c list.c
# ch 5 = ops.h bind.c null.c symbolic.c
# ch 6 = token.h input.c lex.c output.c
# ch 7, 8, 9, 10, 11 = error.c expr.c tree.c enode.c expr.c simp.c stmt.c decl.c main.c init.c
# ch 12 = dag.c
# _____ = event.c trace.c prof.c profio.c
# ch 13 = config.h
# ch 13, 14, 15 = gen.c
# ch 16 = mips.md
# ch 17 = sparc.md
# ch 18 = x86.md