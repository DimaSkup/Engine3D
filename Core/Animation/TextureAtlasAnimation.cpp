#include "TextureAtlasAnimation.h"


TextureAtlasAnimation::TextureAtlasAnimation()
{
}



void TextureAtlasAnimation::Initialize(
	const UINT textureRows,
	const UINT textureColumns,
	const float animDuration)
{
	assert(textureRows > 0);
	assert(textureColumns > 0);
	assert(animDuration > 0.0f);

	texFramesCount_ = textureRows * textureColumns;
	timeStep_ = animDuration / texFramesCount_;  // duration of one animation frame
	std::vector<DirectX::XMFLOAT2> texTranslations(texFramesCount_);

	texRows_ = textureRows;
	texColumns_ = textureColumns;
	animDuration_ = animDuration;
	texCellWidth_ = 1.0f / textureColumns;
	texCellHeight_ = 1.0f / textureRows;
	

	// compute texture translations for animation
	for (UINT i = 0; i < textureRows; ++i)
	{
		for (UINT j = 0; j < textureColumns; ++j)
		{
			const UINT idx = (i * textureColumns) + j;
			texTranslations[idx].x = texCellWidth_ * j;
			texTranslations[idx].y = texCellHeight_ * i;
		}
	}

	// store computed texture translations data
	textureTranslations_ = texTranslations;
}

///////////////////////////////////////////////////////////

void TextureAtlasAnimation::Update(
	const float deltaTime, 
	_Out_ DirectX::XMMATRIX & texTransform)
{
	// increase current animation time
	currAnimTime_ += deltaTime;

	// check if we have to change a frame of the animation
	if (currAnimTime_ > timeStep_)
	{
		currAnimTime_ = 0.0f;
		++currTexFrameIdx_;


		if (currTexFrameIdx_ >= texFramesCount_)
			currTexFrameIdx_ = 0;

		// translate texture
		const DirectX::XMMATRIX texTrans = DirectX::XMMatrixTranslation(
			textureTranslations_[currTexFrameIdx_].x,
			textureTranslations_[currTexFrameIdx_].y,
			0.0f);

		// scale texture
		const DirectX::XMMATRIX texScale = DirectX::XMMatrixScaling(
			texCellWidth_,
			texCellHeight_,
			0.0f);

		// compute final transformation of the texture for this animation frame
		texTransform = texScale * texTrans;
	}
}