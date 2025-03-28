#pragma once

#include "common.h"
#include "directx.h"

#include "RootSignatureEnums.h"

#include "Log.h"

namespace graphics
{
    namespace dx12
    {
        class RootParameter
        {
        public:
            RootParameter()
            {
                clear();
            }

            ~RootParameter()
            {
                clear();
            }

            void clear()
            {
                mDescriptorRanges.clear();
                mRootParam.ParameterType = (D3D12_ROOT_PARAMETER_TYPE)0xFFFFFFFF;
            }

            void initAsConstants(unsigned int shaderRegister, unsigned int dwordsCount, D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL, unsigned int registerSpace = 0)
            {
                mRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
                mRootParam.ShaderVisibility = shaderVisibility;
                mRootParam.Constants.Num32BitValues = dwordsCount;
                mRootParam.Constants.ShaderRegister = shaderRegister;
                mRootParam.Constants.RegisterSpace = registerSpace;
            }

            void initAsCBV(unsigned int shaderRegister, D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL, unsigned int registerSpace = 0)
            {
                mRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
                mRootParam.ShaderVisibility = shaderVisibility;
                mRootParam.Descriptor.ShaderRegister = shaderRegister;
                mRootParam.Descriptor.RegisterSpace = registerSpace;
            }

            void initAsSRV(unsigned int shaderRegister, D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL, unsigned int registerSpace = 0)
            {
                mRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
                mRootParam.ShaderVisibility = shaderVisibility;
                mRootParam.Descriptor.ShaderRegister = shaderRegister;
                mRootParam.Descriptor.RegisterSpace = registerSpace;
            }

            void initAsUAV(unsigned int shaderRegister, D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL, unsigned int registerSpace = 0)
            {
                mRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
                mRootParam.ShaderVisibility = shaderVisibility;
                mRootParam.Descriptor.ShaderRegister = shaderRegister;
                mRootParam.Descriptor.RegisterSpace = registerSpace;
            }

            void initAsDescriptorTable(unsigned int rangeCount, D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL)
            {
                mRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
                mRootParam.ShaderVisibility = shaderVisibility;
                mRootParam.DescriptorTable.NumDescriptorRanges = rangeCount;

                mDescriptorRanges.resize(rangeCount);

                mRootParam.DescriptorTable.pDescriptorRanges = mDescriptorRanges.data();
            }

            void setTableRange(unsigned int rangeIndex, D3D12_DESCRIPTOR_RANGE_TYPE rangeType, unsigned int shaderRegister, unsigned int count, unsigned int registerSpace = 0)
            {
                HASSERT(rangeIndex < mDescriptorRanges.size(), "Descriptor table range out of bounds.");

                D3D12_DESCRIPTOR_RANGE& range = mDescriptorRanges[rangeIndex];
                range.RangeType = rangeType;
                range.NumDescriptors = count;
                range.BaseShaderRegister = shaderRegister;
                range.RegisterSpace = registerSpace;
                range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
            }

            const D3D12_ROOT_PARAMETER& operator()() const { return mRootParam; }

        private:
            D3D12_ROOT_PARAMETER mRootParam;
            std::vector<D3D12_DESCRIPTOR_RANGE> mDescriptorRanges;
        };
    }
}
