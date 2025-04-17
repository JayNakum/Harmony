#pragma once

#include "common.h"
#include "directx.h"

#include "RootSignatureEnums.h"

#include "types/RootParameterType.h"
#include "types/DescriptorRangeType.h"
#include "types/ShaderVisibility.h"
#include "types/Filter.h"
#include "types/TextureAddressMode.h"
#include "types/ComparisionFunction.h"
#include "types/StaticBorderColor.h"

#include "Log.h"

namespace graphics
{
    namespace dx12
    {
        /*
        * Wrapper over D3D12_ROOT_PARAMETER
        * The class is responsible for initializing each parameter.
        * Theoretically it is possible to reuse a parameter, but don't.
        */
        class RootParameter
        {
        public:
            RootParameter()
            {
                mRootParam.ParameterType = (D3D12_ROOT_PARAMETER_TYPE)0xFFFFFFFF;
            }

            // Initialize as 32 bit constants.
            inline void initAsConstants(const unsigned int shaderRegister, const unsigned int dwordsCount, const ShaderVisibility visibility = ShaderVisibility::All, const unsigned int registerSpace = 0)
            {
                mRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
                mRootParam.ShaderVisibility = toD3DShaderVisibility(visibility);
                mRootParam.Constants.Num32BitValues = dwordsCount;
                mRootParam.Constants.ShaderRegister = shaderRegister;
                mRootParam.Constants.RegisterSpace = registerSpace;
            }

            // A generic method for all type of ShaderViews (CBV, SRV, UAV)
            // Calling this method with RootParameterType::DescriptorTable is not allowed.
            inline void initAsShaderView(const RootParameterType type, const unsigned int shaderRegister, const ShaderVisibility visibility = ShaderVisibility::All, const unsigned int registerSpace = 0)
            {
                HASSERT(type != RootParameterType::DescriptorTable, "Invalid root parameter type. Please use initAsDescriptorTable() instead.");

                mRootParam.ParameterType = toD3DRootParameterType(type);
                mRootParam.ShaderVisibility = toD3DShaderVisibility(visibility);
                mRootParam.Descriptor.ShaderRegister = shaderRegister;
                mRootParam.Descriptor.RegisterSpace = registerSpace;
            }

            // Initializes the parameter as descriptor table with rangeCount entries.
            // Each entry in the table is a descriptor range, which are initialized via setTableRange().
            inline void initAsDescriptorTable(const size_t rangeCount, const ShaderVisibility visibility = ShaderVisibility::All)
            {
                mRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
                mRootParam.ShaderVisibility = toD3DShaderVisibility(visibility);
                mRootParam.DescriptorTable.NumDescriptorRanges = (UINT)rangeCount;

                mDescriptorRanges.resize(rangeCount);
                mDescriptorRanges.shrink_to_fit();

                mRootParam.DescriptorTable.pDescriptorRanges = mDescriptorRanges.data();
            }

            // Initializes each entry of the descriptor table.
            // It is mandatory to initialize the root parameter as descriptor table first.
            inline void setTableRange(const unsigned int rangeIndex, const DescriptorRangeType type, const unsigned int shaderRegister, const unsigned int count, const unsigned int registerSpace = 0)
            {
                HASSERT(rangeIndex < mDescriptorRanges.size(), "Descriptor table range out of bounds. Did you call init?");

                D3D12_DESCRIPTOR_RANGE& range = mDescriptorRanges[rangeIndex];
                range.RangeType = toD3DDescriptorRangeType(type);
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

    namespace dx12
    {
        /*
        * Wrapper for static samplers
        */
        class StaticSampler
        {
        public:
            // StaticSamplerDescription with default values.
            // The default values are as per CD3DX12_STATIC_SAMPLER_DESC
            struct Description
            {
                sampler::Filter filter = sampler::Filter::Anisotropic;
                sampler::TextureAddressMode addressU = sampler::TextureAddressMode::Wrap;
                sampler::TextureAddressMode addressV = addressU;
                sampler::TextureAddressMode addressW = addressV;
                float mipLODBias = 0;
                unsigned int maxAnisotropy = 16;
                sampler::ComparisionFunction comparisonFunc = sampler::ComparisionFunction::LessEqual;
                sampler::StaticBorderColor borderColor = sampler::StaticBorderColor::OpaqueWhite;
                float minLOD = 0.0f;
                float maxLOD = FLT_MAX;
            };

            inline void initStaticSampler(const unsigned int shaderRegister, const Description& description = {}, const ShaderVisibility visibility = ShaderVisibility::All, const unsigned int registerSpace = 0)
            {
                mStaticSampler.ShaderRegister = shaderRegister;
                mStaticSampler.RegisterSpace = registerSpace;
                mStaticSampler.ShaderVisibility = toD3DShaderVisibility(visibility);

                mStaticSampler.Filter = sampler::dx12::toD3DFilter(description.filter);
                mStaticSampler.AddressU = sampler::dx12::toD3DTextureAddressMode(description.addressU);
                mStaticSampler.AddressV = sampler::dx12::toD3DTextureAddressMode(description.addressV);
                mStaticSampler.AddressW = sampler::dx12::toD3DTextureAddressMode(description.addressW);
                mStaticSampler.MipLODBias = description.mipLODBias;
                mStaticSampler.MaxAnisotropy = description.maxAnisotropy;
                mStaticSampler.ComparisonFunc = sampler::dx12::toD3DComparisonFunc(description.comparisonFunc);
                mStaticSampler.BorderColor = sampler::dx12::toD3DStaticBorderColor(description.borderColor);
                mStaticSampler.MinLOD = description.minLOD;
                mStaticSampler.MaxLOD = description.maxLOD;
            }

            const D3D12_STATIC_SAMPLER_DESC& operator()() const { return mStaticSampler; }

        private:
            D3D12_STATIC_SAMPLER_DESC mStaticSampler;
        };
    }
}
