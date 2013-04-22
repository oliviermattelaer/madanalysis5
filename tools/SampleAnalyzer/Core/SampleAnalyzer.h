////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012 Eric Conte, Benjamin Fuks, Guillaume Serret
//  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
//  
//  This file is part of MadAnalysis 5.
//  Official website: <http://madanalysis.irmp.ucl.ac.be>
//  
//  MadAnalysis 5 is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  MadAnalysis 5 is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with MadAnalysis 5. If not, see <http://www.gnu.org/licenses/>
//  
////////////////////////////////////////////////////////////////////////////////


#ifndef SAMPLE_ANALYZER_H
#define SAMPLE_ANALYZER_H

// STL headers
#include <iostream>
#include <string>
#include <vector>


// SampleAnalyzer headers
#include "SampleAnalyzer/Core/StatusCode.h"
#include "SampleAnalyzer/Core/Configuration.h"
#include "SampleAnalyzer/Service/LogService.h"
#include "SampleAnalyzer/DataFormat/EventFormat.h"
#include "SampleAnalyzer/DataFormat/SampleFormat.h"
#include "SampleAnalyzer/Reader/ReaderManager.h"
#include "SampleAnalyzer/Analyzer/AnalyzerManager.h"
#include "SampleAnalyzer/Filter/FilterManager.h"
#include "SampleAnalyzer/Writer/WriterManager.h"
#include "SampleAnalyzer/JetClustering/JetClustererManager.h"


namespace MA5
{

class SampleAnalyzer
{
 private :
 
  std::string analysisName_; 
  std::string datasetName_;

  /// Configuration of SampleAnalyzer
  Configuration cfg_;

  /// List of input files
  std::vector<std::string> inputs_;

  /// List of managers
  WriterManager       fullWriters_;
  ReaderManager       fullReaders_;
  AnalyzerManager     fullAnalyses_;
  FilterManager       fullFilters_;
  JetClustererManager fullJetClusterers_;

  /// List of managers
  std::vector<WriterBase*>       writers_;
  std::vector<ReaderBase*>       readers_;
  std::vector<AnalyzerBase*>     analyzers_;
  std::vector<FilterBase*>       filters_;
  std::vector<JetClustererBase*> clusters_;

  /// Reading status
  unsigned int file_index_;
  bool next_file_;

  /// Counters
  std::vector<ULong64_t> counter_read_;
  std::vector<ULong64_t> counter_passed_;

  /// The only one pointer to the reader
  ReaderBase* myReader_;
  
 public:

  /// Constructor withtout arguments
  SampleAnalyzer();

  /// Initialization of the SampleAnalyzer
  bool Initialize(int argc, char **argv, const std::string& filename);

  /// Getting pointer to an analyzer
  AnalyzerBase* InitializeAnalyzer(const std::string& name, 
                                   const std::string& outputname,
                        const std::map<std::string,std::string>& parameters);

  /// Getting pointer to a filter
  FilterBase* InitializeFilter(const std::string& name, 
                               const std::string& outputname,
                        const std::map<std::string,std::string>& parameters);

  /// Getting pointer to a writer
  WriterBase* InitializeWriter(const std::string& name, 
                               const std::string& outputname);

  /// Getting pointer to a jet clusterer
  JetClustererBase* InitializeJetClusterer(const std::string& name, 
                  const std::map<std::string,std::string>& parameters);

  /// Reading the next event
  StatusCode::Type NextEvent(SampleFormat& mysample, EventFormat& myevent);

  /// Reading the next file
  StatusCode::Type NextFile(SampleFormat& mysample);

  /// Finalization of the SampleAnalyzer
  bool Finalize(std::vector<SampleFormat>& mysamples, EventFormat& myevent);

 private:

  /// Filling the summary format
  void FillSummary(SampleFormat& summary,
                   const std::vector<SampleFormat>& mysamples);

};

}

#endif
