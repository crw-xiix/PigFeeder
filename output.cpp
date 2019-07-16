//Must output the website with no external refs, just plain html...
//Use % % % (with no spaces) to represent a line converted, it will be repeated
//Use % % (with no spaces) to define a variable string replacement

#include "output.h"

void outputSite(void (*printFunction)(const char *) ) {
	printFunction("<!doctype html>");
	printFunction("<html lang=\"en\">");
	printFunction("<head>");
	printFunction("    <meta charset=\"utf-8\">");
	printFunction("    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1, shrink-to-fit=no\">");
	printFunction("    <link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.1.3/css/bootstrap.min.css\" integrity=\"sha384-MCw98/SFnGE8fJT3GXwEOngsV7Zt27NXFoaoApmYm81iuXoPkFOJwJ8ERdknLPMO\" crossorigin=\"anonymous\">");
	printFunction("    <title>{%1}</title>");
	printFunction("    <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>");
	printFunction("    <script src=\"https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.3/umd/popper.min.js\" integrity=\"sha384-ZMP7rVo3mIykV+2+9J3UJ46jBk0WLaUAdn689aCwoqbBJiSnjAK/l8WvCWPIPm49\" crossorigin=\"anonymous\"></script>");
	printFunction("    <script src=\"https://stackpath.bootstrapcdn.com/bootstrap/4.1.3/js/bootstrap.min.js\" integrity=\"sha384-ChfqqxuZUCnJSK3+MXmPNIyE6ZbWh2IMqE241rYiqJxyMiZ6OW/JmZQ5stwEULTy\" crossorigin=\"anonymous\"></script>");
	printFunction("    <script>");
	printFunction("        function openButton() {");
	printFunction("            console.log(\"Trying to Open\");");
	printFunction("            $.get(\"/Open\", function (data) {");
	printFunction("            });");
	printFunction("        }");
	printFunction("        function closeButton() {");
	printFunction("            console.log(\"Trying to Close\");");
	printFunction("            $.get(\"/Close\", function (data) {");
	printFunction("            });");
	printFunction("        }");
	printFunction("        function cycleButton() {");
	printFunction("            console.log(\"Trying to Cycle\");");
	printFunction("            $.get(\"/Cycle\", function (data) {");
	printFunction("            });");
	printFunction("        }");
	printFunction("        function danceButton() {");
	printFunction("            console.log(\"Trying to Dance\");");
	printFunction("            $.get(\"/Dance\", function (data) {");
	printFunction("            });");
	printFunction("        }");
	printFunction("");
	printFunction("        function auxOn() {");
	printFunction("            console.log(\"Aux On\");");
	printFunction("            $.get(\"/AuxOn\", function (data) {");
	printFunction("            });");
	printFunction("        }");
	printFunction("");
	printFunction("        function auxOff() {");
	printFunction("            console.log(\"Aux Off\");");
	printFunction("            $.get(\"/AuxOff\", function (data) {");
	printFunction("            });");
	printFunction("        }");
	printFunction("");
	printFunction("        function refresh() {");
	printFunction("           /* $.getJSON(\"/data.json\", function (data) {");
	printFunction("                console.log(data);");
	printFunction("            });*/");
	printFunction("            $.get(\"/log.json\", function (data) {");
	printFunction("                $(\"#dLog\").html(data);");
	printFunction("            });");
	printFunction("        }");
	printFunction("");
	printFunction("        $(document).ready(function () {");
	printFunction("            $(\"#bCycle\").click(cycleButton);");
	printFunction("            $(\"#bOpen\").click(openButton);");
	printFunction("            $(\"#bClose\").click(closeButton);");
	printFunction("            $(\"#bDance\").click(danceButton);");
	printFunction("            $(\"#bAuxOn\").click(auxOn);");
	printFunction("            $(\"#bAuxOff\").click(auxOff);");
	printFunction("            refresh();");
	printFunction("            setInterval(refresh, 3000);");
	printFunction("        });");
	printFunction("");
	printFunction("    </script>");
	printFunction("    <style>");
	printFunction("    </style>");
	printFunction("");
	printFunction("</head>");
	printFunction("<body>   ");
	printFunction("    <div class=\"container text-center\">");
	printFunction("        <div class=\"jumbotron\">");
	printFunction("            <h1>{%1}</h1>");
	printFunction("            <h2>{%2}</h2>");
	printFunction("            <div class=\"row\">");
	printFunction("                <div class=\"col\">");
	printFunction("                    <button type=\"button\" class=\"btn btn-success\" id=\"bCycle\">Cycle Feeder</button>");
	printFunction("                </div>");
	printFunction("                <div class=\"col\">");
	printFunction("                    <button type=\"button\" class=\"btn btn-warning\" id=\"bOpen\">Open Feed Tube</button>");
	printFunction("                </div>");
	printFunction("                <div class=\"col\">");
	printFunction("                    <button type=\"button\" class=\"btn  btn-warning\" id=\"bClose\">Close Feed Tube</button>");
	printFunction("                </div>");
	printFunction("                <div class=\"col\">");
	printFunction("                    <button type=\"button\" class=\"btn btn-danger\" id=\"bDance\">Cmon now lets Dance!</button>");
	printFunction("                </div>");
	printFunction("            </div>");
	printFunction("            <br />");
	printFunction("            <div class=\"row\">");
	printFunction("                <div class=\"col\">");
	printFunction("                    <button type=\"button\" class=\"btn btn-success\" id=\"bAuxOn\">Aux On</button>");
	printFunction("                </div>");
	printFunction("                <div class=\"col\">");
	printFunction("                    <button type=\"button\" class=\"btn btn-warning\" id=\"bAuxOff\">Aux Off</button>");
	printFunction("                </div>");
	printFunction("                <div class=\"col\">");
	printFunction("                </div>");
	printFunction("                <div class=\"col\">");
	printFunction("                    <a href=\"/Edit\" class=\"btn btn-info\">Edit Config</a>");
	printFunction("                </div></div></div><div id=\"dLog\"></div></div>");
	printFunction("</body>");
	printFunction("</html>");
}

