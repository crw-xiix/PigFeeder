//Must output the website with no external refs, just plain html...
//Use % % % (with no spaces) to represent a line converted, it will be repeated
#include "outputedit.h"

void outputEditSite(void (*printFunction)(const char *) ) {
	printFunction("<!doctype html>");
	printFunction("");
	printFunction("");
	printFunction("");
	printFunction("");
	printFunction("<html lang=\"en\">");
	printFunction("<head>");
	printFunction("    <meta charset=\"utf-8\">");
	printFunction("    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1, shrink-to-fit=no\">");
	printFunction("    <link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.1.3/css/bootstrap.min.css\" integrity=\"sha384-MCw98/SFnGE8fJT3GXwEOngsV7Zt27NXFoaoApmYm81iuXoPkFOJwJ8ERdknLPMO\" crossorigin=\"anonymous\">");
	printFunction("    <title>CRW System Monitor</title>");
	printFunction("    <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>");
	printFunction("    <script src=\"https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.3/umd/popper.min.js\" integrity=\"sha384-ZMP7rVo3mIykV+2+9J3UJ46jBk0WLaUAdn689aCwoqbBJiSnjAK/l8WvCWPIPm49\" crossorigin=\"anonymous\"></script>");
	printFunction("    <script src=\"https://stackpath.bootstrapcdn.com/bootstrap/4.1.3/js/bootstrap.min.js\" integrity=\"sha384-ChfqqxuZUCnJSK3+MXmPNIyE6ZbWh2IMqE241rYiqJxyMiZ6OW/JmZQ5stwEULTy\" crossorigin=\"anonymous\"></script>");
	printFunction("");
	printFunction("");
	printFunction("    <script>");
	printFunction("        String.prototype.formatUnicorn = String.prototype.formatUnicorn ||");
	printFunction("            function () {");
	printFunction("                \"use strict\";");
	printFunction("                var str = this.toString();");
	printFunction("                if (arguments.length) {");
	printFunction("                    var t = typeof arguments[0];");
	printFunction("                    var key;");
	printFunction("                    var args = (\"string\" === t || \"number\" === t) ?");
	printFunction("                        Array.prototype.slice.call(arguments)");
	printFunction("                        : arguments[0];");
	printFunction("");
	printFunction("                    for (key in args) {");
	printFunction("                        str = str.replace(new RegExp(\"\\\\{\" + key + \"\\\\}\", \"gi\"), args[key]);");
	printFunction("                        ");
	printFunction("");
	printFunction("                    }");
	printFunction("                }");
	printFunction("");
	printFunction("                return str;");
	printFunction("            };");
	printFunction("");
	printFunction("        function refresh() {");
	printFunction("            $.getJSON(\"/data.json\", function (data) {");
	printFunction("                console.log(data);");
	printFunction("                if (data.moving > 0) {");
	printFunction("                    animateOn();");
	printFunction("                } else {");
	printFunction("                    animateOff();");
	printFunction("                }");
	printFunction("            });");
	printFunction("        }");
	printFunction("");
	printFunction("        function timerHit() {");
	printFunction("//            loadIt();");
	printFunction("  //          logButton();");
	printFunction("        }");
	printFunction("");
	printFunction("        function loadIt() {");
	printFunction("            $.getJSON(\"/data.json\", function (data) {");
	printFunction("                /*");
	printFunction("                gauges[0].value = data.Temp;");
	printFunction("                gauges[1].value = (data.Water);");
	printFunction("                gauges[2].value = (data.Time);");
	printFunction("                gauges[3].value = (data.LastOutTemp);");
	printFunction("                gauges[4].value = (data.LastRemoteTemp);");
	printFunction("                gauges[5].value = (data.ThermostatSetting);");
	printFunction("                $(\"#dPump\").html(\"Cooler Pump:\" + data.Pump);");
	printFunction("                $(\"#dFan\").html(\"Cooler Fan:\" + data.Fan);");
	printFunction("                $(\"#dThermo\").html(\"Thermo:\" + data.ThermostatCool); ");
	printFunction("                */");
	printFunction("            });");
	printFunction("        }");
	printFunction("");
	printFunction("        function logButton() {");
	printFunction("            $.get(\"/log.json\", function (data) {");
	printFunction("                $(\"#dLog\").html(data);");
	printFunction("            });");
	printFunction("        }");
	printFunction("        function FillPage() {");
	printFunction("            var i;");
	printFunction("            for (i = 0; i < 20; i++) {");
	printFunction("                var st = '<tr><td><label for=\"task{0}\">Task:</label><select class=\"form-control taskList\" id=\"task{0}\"><option value=\"0\">Nothing</option></select></td><td><label for=\"usr\">Time:</label><input type=\"text\" class=\"form-control\" id=\"time{0}\"></td > <td><label class=\"checkbox-inline\"><input type=\"checkbox\" id=\"sunrise{0}\" value=\"\">Sunrise</label><br /><label class=\"checkbox-inline\"><input type=\"checkbox\" id=\"sunset{0}\" value=\"\">Sunset</label></td><td><label for=\"time{0}\">Option:</label><input type=\"text\" class=\"form-control\" id=\"option{0}\"></td></tr>'.formatUnicorn(i);");
	printFunction("                $(\".DataArea\").append(st);");
	printFunction("            }");
	printFunction("        }");
	printFunction("        function huh() {");
	printFunction("            $.getJSON(\"/schedule.json\", function (data) {");
	printFunction("                console.log(\"asfasdfa\");");
	printFunction("                $(\"#iTitle\").html(data.Title);");
	printFunction("                $(\"#iVersion\").html(data.Version);");
	printFunction("                loadTaskItems(data.TaskItems);");
	printFunction("                loadTaskForm(data.Tasks);");
	printFunction("            });");
	printFunction("        }");
	printFunction("");
	printFunction("        function loadTaskItems(items) {");
	printFunction("            for (i = 0; i < items.length; i++) {");
	printFunction("                $('.taskList').append($('<option>', items[i]));");
	printFunction("            }");
	printFunction("        }");
	printFunction("        function loadTaskForm(items) {");
	printFunction("            for (i = 0; i < items.length; i++) {");
	printFunction("                var st = items[i].task;");
	printFunction("                var objs = st.split(\":\")");
	printFunction("                var ttask = objs[0];");
	printFunction("                var others = objs[1].split(\",\",4);");
	printFunction("                //Now we can set the values on screen");
	printFunction("                $(\"#task\"+i).val(ttask);");
	printFunction("                $(\"#time\" + i).val(others[0]);");
	printFunction("                $(\"#sunrise\" + i).prop('checked', (others[1] != 0));");
	printFunction("                $(\"#sunset\" + i).prop('checked', (others[2] != 0));");
	printFunction("                $(\"#option\" + i).val(others[3]);");
	printFunction("            }");
	printFunction("        }");
	printFunction("");
	printFunction("        function sendData() {");
	printFunction("            var st = \"?\";");
	printFunction("            st += \"Title=\" + encodeURIComponent($(\"#iTitle\").html());");
	printFunction("            st += \"&\";");
	printFunction("            st += \"Version=\" + encodeURIComponent($(\"#iVersion\").html());");
	printFunction("            st += \"&\";");
	printFunction("            for (i = 0; i < 20; i++) {");
	printFunction("                ");
	printFunction("                var t = \"\";");
	printFunction("                var taskSelected = $(\"#task\" + i + \" option:selected\").val();");
	printFunction("                if (taskSelected == 0) continue;");
	printFunction("");
	printFunction("                t = t + taskSelected;");
	printFunction("");
	printFunction("                t = t + \":\" + $(\"#time\" + i).val() + \",\";");
	printFunction("                if ($(\"#sunrise\" + i).prop('checked')) {");
	printFunction("                    t += \"1,\";");
	printFunction("                } else {");
	printFunction("                    t += \"0,\";");
	printFunction("                }");
	printFunction("                if ($(\"#sunset\" + i).prop('checked')) {");
	printFunction("                    t += \"1,\";");
	printFunction("                } else {");
	printFunction("                    t += \"0,\";");
	printFunction("                }");
	printFunction("                t += encodeURIComponent($(\"#option\" + i).val());");
	printFunction("                st += t + \"&\";");
	printFunction("            }");
	printFunction("            console.log(st);");
	printFunction("            $.get(\"/setConfig\" + st, function (data) {");
	printFunction("                //We sent the data.........  wait for reboot");
	printFunction("               // console.log(st);");
	printFunction("//                window.location.href = \"/\";");
	printFunction("            });");
	printFunction("        }");
	printFunction("        ");
	printFunction("");
	printFunction("        $(document).ready(function () {");
	printFunction("            huh();");
	printFunction("            $(\"#bSave\").click(sendData);");
	printFunction("            FillPage();");
	printFunction("        });");
	printFunction("");
	printFunction("    </script>");
	printFunction("");
	printFunction("</head>");
	printFunction("<!-- test -->");
	printFunction("<body>");
	printFunction("    <div class=\"container\">");
	printFunction("        <div class=\"jumbotron\">");
	printFunction("            <h1 id=\"iTitle\">%%Title%%</h1>");
	printFunction("            <h2 id=\"iVersion\">%%Version%%</h2>");
	printFunction("        </div>");
	printFunction("        <div class=\"jumbotron\">");
	printFunction("            Edit Setup");
	printFunction("        </div>");
	printFunction("        <div class=\"well\">");
	printFunction("            <h3>Events</h3>");
	printFunction("            <table class=\"table\">");
	printFunction("                <thead>");
	printFunction("                    <tr>");
	printFunction("                        <th>Task</th>");
	printFunction("                        <th>Time</th>");
	printFunction("                        <th>Offsets</th>");
	printFunction("                        <th>Options</th>");
	printFunction("                        <th>Actions</th>");
	printFunction("                    </tr>");
	printFunction("                </thead>");
	printFunction("                <tbody id=\"tableBody\" class=\"DataArea\"></tbody>");
	printFunction("            </table>");
	printFunction("            <button type=\"button\" class=\"btn btn-success\" id=\"bSave\">Save</button>");
	printFunction("        </div>");
	printFunction("");
	printFunction("");
	printFunction("");
	printFunction("");
	printFunction("        <div>Log File</div>");
	printFunction("        <div id=\"dLog\"></div>");
	printFunction("        <br />");
	printFunction("        <div id=\"dRaw\"></div>");
	printFunction("    </div>");
	printFunction("</body>");
	printFunction("</html>");
	printFunction("");
	printFunction("<!--");
	printFunction("                                <option value=\"0\">Nothing</option>");
	printFunction("                                <option value=\"1\">Open Door</option>");
	printFunction("                                <option value=\"2\">Close Door</option>");
	printFunction("                                <option value=\"3\">Light On</option>");
	printFunction("                                <option value=\"4\">Light Off</option>");
	printFunction("                                <option value=\"5\">Log Message</option>");
	printFunction("<tr><td><label for=\"task%%\">Task:</label><select class=\"form-control\" id=\"task%%\"></select></td><td><label for=\"usr\">Time:</label><input type=\"text\" class=\"form-control\" id=\"time%%\">");
	printFunction("</td><td><label class=\"checkbox-inline\"><input type=\"checkbox\" id=\"sunrise%%\" value=\"\">Sunrise</label><br /><label class=\"checkbox-inline\"><input type=\"checkbox\" id=\"sunset%%\" value=\"\">Sunset</label>");
	printFunction("</td><td><label for=\"time0\">Option:</label><input type=\"text\" class=\"form-control\" id=\"option%%\"></td></tr>");
	printFunction("");
	printFunction("    -->");
}
