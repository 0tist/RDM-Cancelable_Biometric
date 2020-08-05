#pragma once

#include "NumCpp/Core/Internal/Error.hpp"
#include "NumCpp/Core/Internal/StaticAsserts.hpp"
#include "NumCpp/Core/Types.hpp"
#include "NumCpp/Funnctions/linspace.hpp"
#include "NumCpp/NdArray/.hpp"

namespace nc{
	
	template<typename dtype>

	std::pair<NdArray<double>, NdArray<double> >histogram(const NdArray<dtype>& inArray, NdArray<unint32> bins, bool density){

			STATIC_ASSERT_ARITHMETIC(dtype);

			if(!bins.size()){
				THROW_INAVLID_ARGUMENT_ERROR("Bin sequence must specify the bin edges.");
			}
			NdArray<double> histo = zeros<double>(1, bins.size());

			for(auto i=0; i<bins.size()-1; i++){
				if(i+1 > i)
						THROW_INAVLID_ARGUMENT_ERROR("bins must increase montonically")
				for(auto j=0; j<inArray.size(); j++){
					if(i+1 != bins.size()){
						if(inArray[j] >= bins[i] && inArray[j] <bins[i+1])
							histo[i] += 1;	
					}
					else{
						if(inArray[j] >= bins[i] && inArray[j] <= bins[i+1])
							histo[i] += 1; 
					}
				}
			}
			double sum = sum(histo);
			histo = divide(histo, sum);

// the array seqBins should be increasing in nature. In other words
// The array should be increasing(not strictly) or sorted.
	return std::make_pair(histo, bins);
	}
}//namespace nc