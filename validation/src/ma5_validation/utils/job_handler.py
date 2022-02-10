################################################################################
#
#  Copyright (C) 2012-2022 Jack Araz, Eric Conte & Benjamin Fuks
#  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
#
#  This file is part of MadAnalysis 5.
#  Official website: <https://github.com/MadAnalysis/madanalysis5>
#
#  MadAnalysis 5 is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  MadAnalysis 5 is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with MadAnalysis 5. If not, see <http://www.gnu.org/licenses/>
#
################################################################################


import os
from typing import Text, Sequence, Union

from ma5_validation.system.exceptions import (
    MadAnalysis5Error,
    InvalidSyntax,
    MadAnalysis5ExecutionError,
)
from .path_handler import PathHandler
import warnings

from .script_handler import ScriptReader


class JobHandler:
    """
    Execute and analyse the MadAnalysis 5 jobs.

    Parameters
    ----------
    script : ScriptReader
        Parsed madanalysis script to be executed
    paths : PathHandler
        Validated paths.
    debug: bool
        Enable or disable debug mode. Default True.
    """

    def __init__(self, script: ScriptReader, paths: PathHandler = None, debug: bool = True):
        assert isinstance(script, ScriptReader), f"Unknown input type: {type(script)}"
        self.script = script
        self.debug = debug

        if paths is None:
            self.ma5_path = PathHandler.MA5PATH
            self.log_path = PathHandler.LOGPATH
        else:
            self.ma5_path = paths.MA5PATH
            self.log_path = paths.LOGPATH
        self.log_file = os.path.join(self.log_path, self.script.name + ".log")

    def write_ma5script(self, commands: Text, name: Text = None) -> None:
        """
        Write dedicated MadAnalysis 5 script
        Parameters
        ----------
        commands : Text
            Commands to be executed
        name : Text
            Name of the script
        """
        script_name = os.path.join(
            self.log_path, name if name is not None else self.script.name + ".ma5"
        )
        with open(script_name, "w") as script:
            script.write(commands)

    def execute(self) -> bool:
        """
        Execute MadAnalysis 5 script
        """
        if not self.script.IsExpert:
            self.write_ma5script(self.script.commands)

            commands = [
                self.ma5_path + "/bin/ma5",
                "--forced",
                "--script",
                # "--debug",
                self.script.mode_flag(),
                os.path.join(self.log_path, self.script.name + ".ma5"),
                "&>",
                self.log_file,
            ]
            if self.debug:
                commands.insert(3, "--debug")

            print("   * Running MadAnalysis 5: " + self.script.title)
            try:
                os.system(" ".join(commands))
            except Exception as err:
                log_file = ""
                with open(self.log_file, "r", encoding="utf-8") as log:
                    log_file = log.read()
                raise MadAnalysis5ExecutionError(
                    f"A problem has occured during MadAnalysis 5 execution\n\n{err}\n\n{log_file}"
                )
        else:
            self.write_ma5script(self.script.commands)
            commands = [
                self.ma5_path + "/bin/ma5",
                "--forced",
                "--script",
                # "--debug",
                self.script.mode_flag(),
                os.path.join(self.log_path, self.script.name),
                self.script.expert_name,
                os.path.join(self.log_path, self.script.name + ".ma5"),
                "&>",
                self.log_file,
            ]

            print("   * Running MadAnalysis 5: " + self.script.title)
            try:
                os.system(" ".join(commands))
                curdir = os.getcwd()
                os.chdir(os.path.join(self.log_path, self.script.name, "Build"))
                os.system("source setup.sh")
                with open(
                    os.path.join(self.log_path, self.script.name, "Input/_defaultset.list"), "w"
                ) as inputs:
                    inputs.write("\n".join(self.script.sample))

                # Copy analysis files
                command = (
                    "cp "
                    + self.script.cpp
                    + " "
                    + os.path.join(
                        self.log_path, self.script.name, "Build/SampleAnalyzer/User/Analyzer"
                    )
                    + "/."
                )
                print("   * Running: " + command)
                os.system(command)

                command = (
                    "cp "
                    + self.script.header
                    + " "
                    + os.path.join(
                        self.log_path, self.script.name, "Build/SampleAnalyzer/User/Analyzer"
                    )
                    + "/."
                )
                print("   * Running: " + command)
                os.system(command)

                # Execute
                os.system(
                    "source setup.sh && make clean all &> compilation.log && "
                    + "./MadAnalysis5job "
                    + os.path.join(self.log_path, self.script.name, "Input/_defaultset.list")
                    + self.script.command_line
                    + " &> "
                    + self.log_file
                )
            except Exception as err:
                log_file = ""
                with open(self.log_file, "r", encoding="utf-8") as log:
                    log_file = log.read()
                raise MadAnalysis5ExecutionError(
                    f"A problem has occured during MadAnalysis 5 execution\n\n{err}\n\n{log_file}"
                )

        return True

    def check(self):
        """
        Check the log file for errors

        Raises
        ------
        MadAnalysis5Error
            If MadAnalysis 5 raised an error during the execution.
        """
        endTag = False
        errorTag = False
        log_file = None
        with open(self.log_file, "r", encoding="utf-8") as log:
            for line in log:
                if line.find("ma5>#END") != -1:
                    endTag = True
                if endTag or self.script.IsExpert:
                    if line.find("ERROR") != -1 or line.find("MA5-ERROR") != -1:
                        errorTag = True
                        log_file = log.read()
                        break
        if not endTag and not self.script.IsExpert:
            InvalidSyntax("   * Can not find the end of the script.")

        if errorTag:
            raise MadAnalysis5Error(
                f"MadAnalysis has raised an error. For details, please see: {self.log_file}"
                f"\n\n\n{log_file}"
            )

        # TODO: This checker needs an numerical validation as well. Currently it only checks
        # if there is a problem with the execution of the scripts i.e. if SampleAnalyzer or python
        # interface raises an error.

        return True
