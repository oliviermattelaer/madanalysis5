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


#ifndef TAGGERBASE_H
#define TAGGERBASE_H

// SampleAnalyser headers
#include "SampleAnalyzer/DataFormat/EventFormat.h"
#include "SampleAnalyzer/DataFormat/SampleFormat.h"
#include "SampleAnalyzer/DataFormat/RecJetFormat.h"
#include "SampleAnalyzer/Service/Physics.h"
#include "SampleAnalyzer/Service/PDGService.h"

// STL headers
#include <algorithm>

namespace MA5
{

class TaggerBase
{
//---------------------------------------------------------------------------------
//                                 data members
//---------------------------------------------------------------------------------
  protected :

    /// Method used
    Int_t Method_;

    /// Delta R max
    Float_t DeltaRmax_;

    /// Is the tagging exclusive ?
    Bool_t Exclusive_;

//---------------------------------------------------------------------------------
//                                method members
//---------------------------------------------------------------------------------
  public :

    /// Constructor without argument
    TaggerBase() 
    {Method_=1; DeltaRmax_=0.5; Exclusive_=false;}

    /// Constructor
    TaggerBase(Int_t Method, Double_t DeltaRmax, Bool_t Exclusive) 
    {Method_=Method; DeltaRmax_=DeltaRmax; Exclusive_=Exclusive;}

    /// Destructor
    ~TaggerBase()
    {}

    /// Matching using dr
    virtual void Method1(SampleFormat& mySample, EventFormat& myEvent)=0;

    /// Matching using the history
    virtual void Method2(SampleFormat& mySample, EventFormat& myEvent)=0;

    /// Matching using a jet preselection with the history before calculating dr
    virtual void Method3(SampleFormat& mySample, EventFormat& myEvent)=0;

    /// Matching general method
    virtual void Execute(SampleFormat& mySample, EventFormat& myEvent)=0;

    /// Accessor to the selected method
    Int_t GetMethod() 
    {return Method_;}

    /// 
    Bool_t IsLast(MCParticleFormat* part, EventFormat& myEvent);

    /// Set a parameter
    virtual void SetParameter(const std::string& key, const std::string& value);
};

}

#endif
