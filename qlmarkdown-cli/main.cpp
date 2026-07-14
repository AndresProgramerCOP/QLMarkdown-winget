#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include <cstring>
#include <algorithm>
#include <sys/stat.h>

#ifdef _WIN32
#include <io.h>
#include <direct.h>
#define access _access
#define getcwd _getcwd
#define chdir _chdir
#define F_OK 0
#define R_OK 4
#define PATH_MAX 260
#else
#include <unistd.h>
#include <limits.h>
#endif

// Include cmark headers
extern "C" {
#include "../cmark-gfm/src/cmark-gfm.h"
#include "../cmark-gfm/extensions/cmark-gfm-core-extensions.h"
#include "../cmark-extra/extensions/extra-extensions.h"
#include "../cmark-extra/extensions/syntaxhighlight.h"
#include "../cmark-extra/extensions/emoji.h"
#include "../cmark-extra/extensions/inlineimage.h"
#include "../cmark-extra/extensions/math_ext.h"
#include "../cmark-extra/extensions/mention.h"
#include "../cmark-extra/extensions/heads.h"
#include "../cmark-extra/extensions/highlight.h"
#include "../cmark-extra/extensions/sub_ext.h"
#include "../cmark-extra/extensions/sup_ext.h"
}

// Embedded default CSS fallback
const std::string DEFAULT_CSS = R"css(
:root {
  /* Base Light */
  --textPrimary: #24292E;
  --background: #ffffff;
  --body-border-color: #dfdfdf;
  --link: #0366d6;
  --link-highlight: rgba(3, 102, 214, 0.08);
  --code-background: #f7f7f9;
  --hr-background: #e3e4e8;
  --thead-background: #f7f7f9;
  --thead-border: #e3e4e8;
  --tr-border: #bfc1c9;
  --tr-alt-background: #f7f7f9;
  --kbd-background: #f7f7f9;
  --kbd-color: #4b4d58;
  --kbd-border: #d6d7dc;
  --blockquote-color: #6a6d7c;
  --blockquote-border: #e3e4e8;
  --heading-color: #17181a;
  --header-bottom-border: #e3e4e8;
  --h6-color: #6a6d7c;
  --frame-border: #e3e4e8;
  --frame-color: #242528;
  --mention-color: #242528;
  --email-toggle-color: #4b4d58;
  --email-toggle-background: #e3e4e8;
  --email-quoted-color: #6a6d7c;
  --keyword-color: #9194a1;
  --code-font: ui-monospace, Menlo, monospace;

  /* Highlight Light */
  --hl_Background: #F6F8FA ;
  --hl_Number: #005cc5 ;
  --hl_Escape: #032f62 ;
  --hl_String: #032f62 ;
  --hl_String_Pre_Processor: #549ef1 ;
  --hl_Block_Comment: #6a737d ;
  --hl_Line_Comment: #6a737d ;
  --hl_Pre_Processor: #d73a49 ;
  --hl_Line_Number:  #52565C ;
  --hl_Operator: #032f62 ;
  --hl_Interpolation:  #79c0ff ;
  --hl_Keyword-1: #d73a49 ;
  --hl_Keyword-2: #d73a49 ;
  --hl_Keyword-3: #24292e ;
  --hl_Keyword-4: #6f42c1 ;
  --hl_Keyword-5: #d73a49 ;
  --hl_Keyword-6: #d73a49 ;
}

@media (prefers-color-scheme: dark) {
  :root {
    /* Base Dark */
  --textPrimary: #c9d1d9;
  --background: #0d1117;
  --body-border-color: #31363C;
  --link: #58a6ff;
  --link-highlight: rgba(3, 102, 214, 0.08);
  --code-background: #2f3438;
  --hr-background: #30363d;
  --thead-background: #f7f7f9;
  --thead-border: #3b434b;
  --tr-border: #3b434b;
  --tr-alt-background: #161c23;
  --kbd-background: #f7f7f9;
  --kbd-color: #4b4d58;
  --kbd-border: #050505;
  --blockquote-color: #8b949e;
  --blockquote-border: #3D434A;
  --heading-color: #17181a;
  --header-bottom-border: #21272c;
  --h6-color: #6a6d7c;
  --frame-border: #ff08d6;
  --frame-color: #242528;
  --mention-color: #242528;
  --email-toggle-color: #4b4d58;
  --email-toggle-background: #e3e4e8;
  --email-quoted-color: #6a6d7c;
  --keyword-color: #9194a1;
  --code-font: ui-monospace, Menlo, monospace;

  /* Highlight Dark */
  --hl_Background: #161b22 ;
  --hl_Number: #79c0ff ;
  --hl_Escape: #a5d6ff ;
  --hl_String: #549ef1 ;
  --hl_String_Pre_Processor: #549ef1 ;
  --hl_Block_Comment: #8b949e ;
  --hl_Line_Comment: #8b949e ;
  --hl_Pre_Processor: #FF7B72 ;
  --hl_Line_Number:  #52565C ;
  --hl_Operator: #79c0ff ;
  --hl_Interpolation:  #79c0ff ;
  --hl_Keyword-1: #FF7B72 ;
  --hl_Keyword-2: #FF7B72 ;
  --hl_Keyword-3: #c9d1d9 ;
  --hl_Keyword-4: #eea8ff ;
  --hl_Keyword-5: #FF7B72 ;
  --hl_Keyword-6: #FF7B72 ;
  }
}

/*Syntax Highlight*/
pre.hl	{ background-color: var(--hl_Background);}
.hl.num { color: var(--hl_Number); font-weight: normal;}
.hl.esc { color: var(--hl_Escape); font-weight: normal;}
.hl.str { color: var(--hl_String); font-weight: normal;}
.hl.pps { color: var(--hl_String_Pre_Processor); font-weight: normal;}
.hl.com { color: var(--hl_Block_Comment); font-weight: normal;}
.hl.slc { color: var(--hl_Line_Comment); font-weight: normal;}
.hl.ppc { color: var(--hl_Pre_Processor); font-weight: normal;}
.hl.lin { color: var(--hl_Line_Number); font-weight: normal;}
.hl.opt { color: var(--hl_Operator); font-weight: normal;}
.hl.ipl { color: var(--hl_Interpolation); font-weight: normal;}
.hl.kwa { color: var(--hl_Keyword-1); font-weight: normal;}
.hl.kwb { color: var(--hl_Keyword-2); font-weight: normal;}
.hl.kwc { color: var(--hl_Keyword-3); font-weight: normal;}
.hl.kwd { color: var(--hl_Keyword-4); font-weight: normal;}
.hl.kwe { color: var(--hl_Keyword-5); font-weight: normal;}
.hl.kwf { color: var(--hl_Keyword-6); font-weight: normal;}

pre {
  max-width: 902px;
}
* {
  box-sizing: border-box;
}
input:disabled {
  touch-action: none;
}
html {
  padding: 0;
  margin: 0;
  -webkit-text-size-adjust: none;
  text-size-adjust: none;
  font-family: -apple-system,BlinkMacSystemFont,Segoe UI,Helvetica,Arial,sans-serif,Apple Color Emoji, Segoe UI Emoji;
}
body {
  margin: 0;
  color: var(--textPrimary);
  background-color: var(--background);
}
@media (min-width: 902px) {
  article {
    border-radius: 8px;
    border-width: 1px;
    border-style: solid;
    border-color: var(--body-border-color);
  }
}
article {
  margin: auto;
  max-width: 902px;
  font-size: inherit;
  line-height: 1.5;
  word-wrap: break-word;
  padding: 32px;
}
a {
  color: var(--link);
  text-decoration: none;
  -webkit-tap-highlight-color: var(--link-highlight);
}
a:hover {
  text-decoration: underline;
}
details summary {
  outline: 0;
}
table {
  border-spacing: 0;
  border-collapse: collapse;
}
blockquote {
  margin: 0;
}
table, table *, pre {
  touch-action: pan-x;
}
.markdown-body ul.contains-task-list, .markdown-body li.task-list-item {
  list-style: none;
  padding-left: 0;
}
pre, code, kbd {
  font-size: 1rem;
  font-family: var(--code-font);
}
.issue-keyword {
  border-bottom: 1px dotted var(--keyword-color);
}
.team-mention, .user-mention {
  font-weight: 600;
  color: var(--mention-color);
  white-space: nowrap;
}
.email-hidden-toggle, .email-hidden-reply {
  display: none;
}
@media (prefers-color-scheme: dark) {
  input[type="checkbox"] {
    filter: invert(85%) brightness(1.5);
  }
}
input[type="checkbox"] {
  transform: translate(0px);
  width: 14px; height: 14px;
}
.markdown-body kbd {
  display: inline-block;
  padding: 0.18rem 0.31rem;
  font-size: 0.7rem;
  line-height: 1.2rem;
  color: var(--kbd-color);
  vertical-align: middle;
  background-color: var(--kbd-background);
  border: 1px solid var(--kbd-border);
  border-radius: 0.25rem;
  box-shadow: inset 0 -1px 0 var(--kbd-border);
  margin-right: 2px;
}
.markdown-body:after, .markdown-body:before {
  display: table;
  content: ""
}
.markdown-body:after {
  clear: both;
}
.markdown-body > :first-child {
  margin-top: 0 !important;
}
.markdown-body > :last-child {
  margin-bottom: 0 !important;
}
.markdown-body a:not([href]) {
  color: inherit;
  text-decoration: none;
}
.markdown-body hr {
  height: .25rem;
  padding: 0;
  margin: 24px 0;
  background-color: var(--hr-background);
  border: 0;
}
.markdown-body blockquote {
  padding-left: 1rem;
  color: var(--blockquote-color);
  border-left: .25rem solid var(--blockquote-border);
}
.markdown-body blockquote > :first-child {
  margin-top: 0;
}
.markdown-body blockquote > :last-child {
  margin-bottom: 0;
}
.markdown-body h1, .markdown-body h2, .markdown-body h3, .markdown-body h4, .markdown-body h5, .markdown-body h6 {
  margin-top: 24px;
  margin-bottom: 16px;
  font-weight: 600;
  line-height: 1.25;
}
.markdown-body h1 code, .markdown-body h1 tt, .markdown-body h2 code, .markdown-body h2 tt, .markdown-body h3 code, .markdown-body h3 tt, .markdown-body h4 code, .markdown-body h4 tt, .markdown-body h5 code, .markdown-body h5 tt, .markdown-body h6 code, .markdown-body h6 tt {
  font-size: inherit;
}
.markdown-body h1 { font-size: 2rem; }
.markdown-body h1, .markdown-body h2 {
  padding-bottom: .3rem;
  border-bottom: 1px solid var(--header-bottom-border);
}
.markdown-body h2 { font-size: 1.5rem; }
.markdown-body h3 { font-size: 1.25rem; }
.markdown-body h4 { font-size: 1rem; }
.markdown-body h5 { font-size: .875rem; }
.markdown-body h6 {
  font-size: .85rem;
  color: var(--h6-color);
}
.markdown-body ol, .markdown-body ul {
  padding-left: 1.5rem;
}
.markdown-body ol.no-list, .markdown-body ul.no-list {
  padding: 0;
  list-style-type: none;
}
.markdown-body ol ol, .markdown-body ol ul, .markdown-body ul ol, .markdown-body ul ul {
  margin-top: 0;
  margin-bottom: 0;
}
.markdown-body li {
  word-wrap: break-all;
}
.markdown-body li > p {
  margin-top: 16px;
}
.markdown-body li + li {
  margin-top: .25rem;
}
.markdown-body dl {
  padding: 0;
}
.markdown-body dl dt {
  padding: 0;
  margin-top: 16px;
  font-size: 1rem;
  font-style: italic;
  font-weight: 600;
}
.markdown-body dl dd {
  padding: 0 16px;
  margin-bottom: 16px;
}
.markdown-body table {
  display: block;
  width: 100%;
  overflow: auto;
}
.markdown-body table th {
  font-weight: 600;
}
.markdown-body table td, .markdown-body table th {
  padding: 6px 13px;
  border: 1px solid var(--thead-border);
}
.markdown-body table tr {
  background-color: var(--background);
}
.markdown-body table tr:nth-child(2n) {
  background-color: var(--tr-alt-background);
}
.markdown-body table img {
  background-color: initial;
}
.markdown-body img {
  max-width: 100%;
  height: auto;
  box-sizing: initial;
  background-color: var(--background);
}
.markdown-body img[align=right] {
  padding-left: 20px;
}
.markdown-body img[align=left] {
  padding-right: 20px;
}
.markdown-body .emoji {
  max-width: none;
  vertical-align: text-top;
  background-color: initial;
}
.markdown-body img.emoji {
    max-width: 18px;
}
.markdown-body code, .markdown-body tt {
  padding: .2rem .4rem;
  margin: 0;
  font-size: 85%;
  background-color: var(--code-background);
  border-radius: 6px;
}
.markdown-body code br, .markdown-body tt br {
  display: none;
}
.markdown-body pre {
  word-wrap: normal;
}
.markdown-body pre > code {
  padding: 0;
  margin: 0;
  font-size: 100%;
  word-break: normal;
  white-space: pre;
  background: transparent;
  border: 0;
}
.markdown-body .highlight {
  margin-bottom: 16px;
}
.markdown-body .highlight pre {
  margin-bottom: 0;
  word-break: normal;
}
.markdown-body .highlight pre, .markdown-body pre {
  padding: 16px;
  overflow: auto;
  font-size: 85%;
  line-height: 1.45;
  background-color: var(--hl_Background);
  border-radius: 6px;
}
.markdown-body pre code, .markdown-body pre tt {
  display: inline;
  max-width: auto;
  padding: 0;
  margin: 0;
  overflow: visible;
  line-height: inherit;
  word-wrap: normal;
  background-color: initial;
  border: 0;
}
.markdown-body mark {
    padding: .2rem;
    background-color: #fcf1b5;
}
@media (prefers-color-scheme: dark) {
    .markdown-body mark {
        padding: .2rem;
        background-color: #fcf8e3;
    }
}
.mermaid {
    text-align: center;
    margin: 16px 0;
    background-color: transparent;
}
.mermaid svg {
    max-width: 100%;
    height: auto;
}
)css";

// CSS parsing helper
std::string getCompleteHTML(const std::string& title, const std::string& body,
                             const std::string& appearance, float baseFontSize,
                             const std::string& customCssCode, bool customCssOverride,
                             bool syntaxHighlight, bool mathEnabled, bool mermaidEnabled) {
    std::string css = DEFAULT_CSS;
    if (!customCssCode.empty()) {
        if (customCssOverride) {
            css = customCssCode;
        } else {
            css += "\n" + customCssCode;
        }
    }

    if (appearance == "light") {
        // Disable prefers-color-scheme: dark media query
        css = std::regex_replace(css, std::regex(R"(@media\s*\(prefers-color-scheme:\s*dark\))"), "@media not all");
    } else if (appearance == "dark") {
        // Force prefers-color-scheme: dark media query
        css = std::regex_replace(css, std::regex(R"(@media\s*\(prefers-color-scheme:\s*dark\))"), "@media all");
    }

    std::string s_header = "";
    std::string s_footer = "";

    if (baseFontSize > 0) {
        s_header += "<style type='text/css'>\nhtml { font-size: " + std::to_string(baseFontSize) + "pt;}\n</style>\n";
    }

    if (mathEnabled) {
        s_header += R"html(
<script type="text/javascript">
MathJax = {
  options: {
    enableMenu: false,
  },
  tex: {
    inlineMath: [
      ['\\(', '\\)']
    ],
    displayMath: [
      ['\\[', '\\]']
    ],
    processEscapes: true,
    processEnvironments: false
  }
};
</script>
)html";
        s_footer += "<script type='text/javascript' id='MathJax-script' src='https://cdn.jsdelivr.net/npm/mathjax@3/es5/tex-mml-chtml.js'></script>\n";
    }

    if (mermaidEnabled) {
        s_footer += "<script type='text/javascript' src='https://cdn.jsdelivr.net/npm/mermaid/dist/mermaid.min.js'></script>\n";
        s_footer += R"html(
<script type="text/javascript">
mermaid.initialize({
startOnLoad: true,
theme: window.matchMedia('(prefers-color-scheme: dark)').matches ? 'dark' : 'default',
securityLevel: 'strict'
});
</script>
)html";
    }

    std::string html = R"html(<!doctype html>
<html>
<head>
<meta charset='utf-8'>
<meta name='viewport' content='width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=0'>
<title>)html" + title + R"html(</title>
<style type='text/css'>
)html" + css + R"html(
</style>
)html" + s_header + R"html(
</head>
<body>
<article class='markdown-body'>
)html" + body + R"html(
</article>
)html" + s_footer + R"html(
</body>
</html>
)html";

    return html;
}

// Math/Mermaid/Task delimiters transformations
std::string swapMathDelimiters(std::string html) {
    std::regex inline_regex(R"((<span class='hl math'[^>]*>)\$([\s\S]*?)\$(</span>))");
    html = std::regex_replace(html, inline_regex, R"($1\($2\)$3)");

    std::regex display_regex(R"((<div class='hl math'[^>]*>)\$\$([\s\S]*?)\$\$(</div>))");
    html = std::regex_replace(html, display_regex, R"($1\[$2\]$3)");
    return html;
}

std::string transformMermaidBlocks(std::string html) {
    std::regex mermaid_regex(R"(<pre[^>]*>\s*<code[^>]*class="[^"]*language-mermaid[^"]*"[^>]*>([\s\S]*?)</code>\s*</pre>)");
    html = std::regex_replace(html, mermaid_regex, R"(<div class="mermaid">$1</div>)");
    return html;
}

std::string addTaskListClasses(std::string html) {
    std::regex task_regex(R"(<li>(\s*<input type="checkbox"[^>]*>))");
    html = std::regex_replace(html, task_regex, R"(<li class='task-list-item'>$1)");
    return html;
}

bool fileExists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

std::string getDirectoryOfFile(const std::string& filepath) {
    size_t found = filepath.find_last_of("/\\");
    if (found != std::string::npos) {
        return filepath.substr(0, found);
    }
    return ".";
}

std::string getFileName(const std::string& filepath) {
    size_t found = filepath.find_last_of("/\\");
    if (found != std::string::npos) {
        return filepath.substr(found + 1);
    }
    return filepath;
}

void printHelp() {
    std::cout << "OVERVIEW: Command line tool to convert markdown files to html (Windows & Cross-platform Port)\n\n"
              << "USAGE: qlmarkdown_cli [<options>] [<files> ...]\n\n"
              << "ARGUMENTS:\n"
              << "  <files>                 File to be processed.\n\n"
              << "MARKDOWN OPTIONS:\n"
              << "  --appearance <appearance>\n"
              << "                          (values: auto, light, dark)\n"
              << "  --base-font-size <number>\n"
              << "                          Set the base font size, in points.\n"
              << "  --footnotes <on|off>    Parse the footnotes. (values: on, off)\n"
              << "  --hard-break <on|off>   Render soft-break elements as hard line breaks. (values: on, off)\n"
              << "  --no-soft-break <on|off>\n"
              << "                          Render soft-break elements as spaces. (values: on, off)\n"
              << "  --raw-html <on|off>     Convert straight quotes to curly. (values: on, off)\n"
              << "  --render-as-code <on|off>\n"
              << "                          Show the plain text file (raw version) instead of the formatted output. (values: on, off)\n"
              << "  --smart-quotes <on|off> Convert straight quotes to curly. (values: on, off)\n"
              << "  --validate-utf8 <on|off>\n"
              << "                          Validate UTF-8 in the input before parsing. (values: on, off)\n"
              << "  --debug <on|off>        Insert in the output some debug information. (values: on, off)\n\n"
              << "MARKDOWN EXTENSIONS:\n"
              << "  --autolink <on|off>     Automatically translate URL/email to link. (values: on, off)\n"
              << "  --emoji <emoji>         Translate the emoji shortcodes. (font, images, off)\n"
              << "  --github-mentions <on|off>\n"
              << "                          Translate mentions to link to the GitHub account. (values: on, off)\n"
              << "  --heads-anchor <on|off> Create anchors for the heads. (values: on, off)\n"
              << "  --highlight <on|off>    Highlight text marked with `==`. (values: on, off)\n"
              << "  --inline-images <on|off>\n"
              << "                          Embed local image files inside the formatted output. (values: on, off)\n"
              << "  --table <on|off>        Enable table extension. (values: on, off)\n"
              << "  --tag-filter <on|off>   Strip potentially dangerous HTML tags. (values: on, off)\n"
              << "  --tasklist <on|off>     Parse task list. (values: on, off)\n"
              << "  --strikethrough <strikethrough>\n"
              << "                          Recognize single/double `~` for the strikethrough style. (single, double, off)\n"
              << "  --syntax-highlight <on|off>\n"
              << "                          Highlight the code inside fenced block. (values: on, off)\n"
              << "  --sub <on|off>          Format subscript characters inside `~` markers. (values: on, off)\n"
              << "  --sup <on|off>          Format superscript characters inside `^` markers. (values: on, off)\n"
              << "  --yaml <yaml>           Render the yaml header. (rmd, all, off)\n\n"
              << "OPTIONS:\n"
              << "  -o <path>               Destination output. If a directory, a new file with the processed source with .html extension is created.\n"
              << "  -v, --verbose           Verbose mode.\n"
              << "  --help, -h              Show help information.\n";
}

int main(int argc, char* argv[]) {
    std::string dest = "";
    bool verbose = false;
    std::string appearance = "auto";
    float baseFontSize = 0.0f;
    bool footnotes = true;
    bool hardBreak = false;
    bool noSoftBreak = false;
    bool rawHtml = true;
    bool renderAsCode = false;
    bool smartQuotes = true;
    bool validateUtf8 = false;
    bool debug = false;

    // Extensions
    bool autolink = true;
    std::string emoji = "font";
    bool githubMentions = false;
    bool headsAnchor = true;
    bool highlight = false;
    bool inlineImages = true;
    bool table = true;
    bool tagFilter = true;
    bool tasklist = true;
    std::string strikethrough = "single";
    bool syntaxHighlight = true;
    bool sub = false;
    bool sup = false;
    std::string yaml = "rmd";

    std::vector<std::string> files;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            printHelp();
            return 0;
        } else if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        } else if (arg == "-o" && i + 1 < argc) {
            dest = argv[++i];
        } else if (arg == "--appearance" && i + 1 < argc) {
            appearance = argv[++i];
        } else if (arg == "--base-font-size" && i + 1 < argc) {
            baseFontSize = std::stof(argv[++i]);
        } else if (arg == "--footnotes" && i + 1 < argc) {
            footnotes = (std::string(argv[++i]) == "on");
        } else if (arg == "--hard-break" && i + 1 < argc) {
            hardBreak = (std::string(argv[++i]) == "on");
        } else if (arg == "--no-soft-break" && i + 1 < argc) {
            noSoftBreak = (std::string(argv[++i]) == "on");
        } else if (arg == "--raw-html" && i + 1 < argc) {
            rawHtml = (std::string(argv[++i]) == "on");
        } else if (arg == "--render-as-code" && i + 1 < argc) {
            renderAsCode = (std::string(argv[++i]) == "on");
        } else if (arg == "--smart-quotes" && i + 1 < argc) {
            smartQuotes = (std::string(argv[++i]) == "on");
        } else if (arg == "--validate-utf8" && i + 1 < argc) {
            validateUtf8 = (std::string(argv[++i]) == "on");
        } else if (arg == "--debug" && i + 1 < argc) {
            debug = (std::string(argv[++i]) == "on");
        } else if (arg == "--autolink" && i + 1 < argc) {
            autolink = (std::string(argv[++i]) == "on");
        } else if (arg == "--emoji" && i + 1 < argc) {
            emoji = argv[++i];
        } else if (arg == "--github-mentions" && i + 1 < argc) {
            githubMentions = (std::string(argv[++i]) == "on");
        } else if (arg == "--heads-anchor" && i + 1 < argc) {
            headsAnchor = (std::string(argv[++i]) == "on");
        } else if (arg == "--highlight" && i + 1 < argc) {
            highlight = (std::string(argv[++i]) == "on");
        } else if (arg == "--inline-images" && i + 1 < argc) {
            inlineImages = (std::string(argv[++i]) == "on");
        } else if (arg == "--table" && i + 1 < argc) {
            table = (std::string(argv[++i]) == "on");
        } else if (arg == "--tag-filter" && i + 1 < argc) {
            tagFilter = (std::string(argv[++i]) == "on");
        } else if (arg == "--tasklist" && i + 1 < argc) {
            tasklist = (std::string(argv[++i]) == "on");
        } else if (arg == "--strikethrough" && i + 1 < argc) {
            strikethrough = argv[++i];
        } else if (arg == "--syntax-highlight" && i + 1 < argc) {
            syntaxHighlight = (std::string(argv[++i]) == "on");
        } else if (arg == "--sub" && i + 1 < argc) {
            sub = (std::string(argv[++i]) == "on");
        } else if (arg == "--sup" && i + 1 < argc) {
            sup = (std::string(argv[++i]) == "on");
        } else if (arg == "--yaml" && i + 1 < argc) {
            yaml = argv[++i];
        } else if (arg[0] == '-') {
            std::cerr << "Unknown option: " << arg << "\n";
            printHelp();
            return 1;
        } else {
            files.push_back(arg);
        }
    }

    if (files.empty()) {
        std::cerr << "Error: No files specified.\n";
        printHelp();
        return 1;
    }

    // Initialize core extensions and extra extensions
    cmark_gfm_core_extensions_ensure_registered();
    cmark_gfm_extra_extensions_ensure_registered();

    // Setup options bitmask
    int options = CMARK_OPT_DEFAULT;
    if (rawHtml) {
        options |= CMARK_OPT_UNSAFE;
    }
    if (hardBreak) {
        options |= CMARK_OPT_HARDBREAKS;
    }
    if (noSoftBreak) {
        options |= CMARK_OPT_NOBREAKS;
    }
    if (validateUtf8) {
        options |= CMARK_OPT_VALIDATE_UTF8;
    }
    if (smartQuotes) {
        options |= CMARK_OPT_SMART;
    }
    if (footnotes) {
        options |= CMARK_OPT_FOOTNOTES;
    }
    if (strikethrough == "double") {
        options |= CMARK_OPT_STRIKETHROUGH_DOUBLE_TILDE;
    }

    for (const auto& filepath : files) {
        if (!fileExists(filepath)) {
            std::cerr << "Error: File not found: " << filepath << "\n";
            return 1;
        }

        std::ifstream infile(filepath, std::ios::in | std::ios::binary);
        if (!infile.is_open()) {
            std::cerr << "Error: Unable to open file: " << filepath << "\n";
            return 1;
        }

        std::stringstream buffer;
        buffer << infile.rdbuf();
        infile.close();

        std::string markdown_text = buffer.str();

        cmark_parser* parser = cmark_parser_new(options);
        if (!parser) {
            std::cerr << "Error: Failed to create cmark parser.\n";
            return 1;
        }

        // Attach syntax extensions
        if (table) {
            if (auto ext = cmark_find_syntax_extension("table")) {
                cmark_parser_attach_syntax_extension(parser, ext);
            }
        }
        if (autolink) {
            if (auto ext = cmark_find_syntax_extension("autolink")) {
                cmark_parser_attach_syntax_extension(parser, ext);
            }
        }
        if (tagFilter) {
            if (auto ext = cmark_find_syntax_extension("tagfilter")) {
                cmark_parser_attach_syntax_extension(parser, ext);
            }
        }
        if (tasklist) {
            if (auto ext = cmark_find_syntax_extension("tasklist")) {
                cmark_parser_attach_syntax_extension(parser, ext);
            }
        }
        if (strikethrough != "off") {
            if (auto ext = cmark_find_syntax_extension("strikethrough")) {
                cmark_parser_attach_syntax_extension(parser, ext);
            }
        }
        if (githubMentions) {
            if (auto ext = cmark_find_syntax_extension("mention")) {
                cmark_parser_attach_syntax_extension(parser, ext);
            }
        }
        if (headsAnchor) {
            if (auto ext = cmark_find_syntax_extension("heads")) {
                cmark_parser_attach_syntax_extension(parser, ext);
            }
        }
        if (highlight) {
            if (auto ext = cmark_find_syntax_extension("highlight")) {
                cmark_parser_attach_syntax_extension(parser, ext);
            }
        }
        if (sub) {
            if (auto ext = cmark_find_syntax_extension("sub")) {
                cmark_parser_attach_syntax_extension(parser, ext);
            }
        }
        if (sup) {
            if (auto ext = cmark_find_syntax_extension("sup")) {
                cmark_parser_attach_syntax_extension(parser, ext);
            }
        }
        if (inlineImages) {
            if (auto ext = cmark_find_syntax_extension("inlineimage")) {
                cmark_parser_attach_syntax_extension(parser, ext);
                std::string baseDir = getDirectoryOfFile(filepath);
                cmark_syntax_extension_inlineimage_set_wd(ext, baseDir.c_str());
            }
        }
        if (emoji != "off") {
            if (auto ext = cmark_find_syntax_extension("emoji")) {
                cmark_syntax_extension_emoji_set_use_characters(ext, emoji == "font");
                cmark_parser_attach_syntax_extension(parser, ext);
            }
        }
        // math extension is always attached if enabled
        bool mathEnabled = true;
        if (auto ext = cmark_find_syntax_extension("math")) {
            cmark_parser_attach_syntax_extension(parser, ext);
        }

        if (syntaxHighlight) {
            if (auto ext = cmark_find_syntax_extension("syntaxhighlight")) {
                cmark_syntax_extension_highlight_set_theme_name(ext, "");
                cmark_syntax_extension_highlight_set_line_number(ext, 0);
                cmark_syntax_extension_highlight_set_tab_spaces(ext, 4);
                cmark_syntax_extension_highlight_set_wrap_limit(ext, 0);
                cmark_parser_attach_syntax_extension(parser, ext);
            }
        }

        // Feed parser
        cmark_parser_feed(parser, markdown_text.c_str(), markdown_text.length());
        cmark_node* doc = cmark_parser_finish(parser);
        if (!doc) {
            std::cerr << "Error: Parsing failed.\n";
            cmark_parser_free(parser);
            return 1;
        }

        // Render HTML
        char* rendered_html = cmark_render_html(doc, options, cmark_parser_get_syntax_extensions(parser));
        std::string body = rendered_html ? rendered_html : "";
        if (rendered_html) {
            free(rendered_html);
        }

        cmark_node_free(doc);
        cmark_parser_free(parser);

        // Delimiter replacements and Mermaid transformations
        body = swapMathDelimiters(body);
        bool hasMermaid = body.find("language-mermaid") != std::string::npos;
        if (hasMermaid) {
            body = transformMermaidBlocks(body);
        }
        if (tasklist && body.find("type=\"checkbox\"") != std::string::npos) {
            body = addTaskListClasses(body);
        }

        std::string final_html = getCompleteHTML(getFileName(filepath), body, appearance, baseFontSize, "", false, syntaxHighlight, true, hasMermaid);

        if (!dest.empty()) {
            std::string outPath = dest;
            // Check if dest is directory (simple check)
            struct stat s;
            if (stat(dest.c_str(), &s) == 0 && (s.st_mode & S_IFDIR)) {
                std::string fname = getFileName(filepath);
                size_t dot = fname.find_last_of(".");
                if (dot != std::string::npos) {
                    fname = fname.substr(0, dot);
                }
                outPath = dest + "/" + fname + ".html";
            }
            std::ofstream outfile(outPath, std::ios::out | std::ios::trunc | std::ios::binary);
            if (!outfile.is_open()) {
                std::cerr << "Error: Unable to write to file: " << outPath << "\n";
                return 1;
            }
            outfile.write(final_html.c_str(), final_html.length());
            outfile.close();
            if (verbose) {
                std::cout << "Successfully rendered: " << filepath << " -> " << outPath << "\n";
            }
        } else {
            std::cout << final_html;
        }
    }

    return 0;
}
