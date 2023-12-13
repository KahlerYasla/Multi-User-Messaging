#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"

#define ANSI_COLOR_RESET "\x1b[0m"

#define LogRed(x) printf(ANSI_COLOR_RED x ANSI_COLOR_RESET)
#define LogGreen(x) printf(ANSI_COLOR_GREEN x ANSI_COLOR_RESET)
#define LogYellow(x) printf(ANSI_COLOR_YELLOW x ANSI_COLOR_RESET)
#define LogBlue(x) printf(ANSI_COLOR_BLUE x ANSI_COLOR_RESET)
#define LogMagenta(x) printf(ANSI_COLOR_MAGENTA x ANSI_COLOR_RESET)
#define LogCyan(x) printf(ANSI_COLOR_CYAN x ANSI_COLOR_RESET)