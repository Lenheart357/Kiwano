// Copyright (c) 2016-2018 Kiwano - Nomango
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#pragma once
#include "../macros.h"
#include "../base/Resource.h"
#include "../2d/include-forwards.h"
#include "../renderer/GifImage.h"
#include "../third-party/tinyxml2/tinyxml2.h"

namespace kiwano
{
	// ��Դ����
	class KGE_API ResourceCache
		: public Singleton<ResourceCache>
	{
		KGE_DECLARE_SINGLETON(ResourceCache);

	public:
		// �� JSON �ļ�������Դ��Ϣ
		bool LoadFromJsonFile(String const& file_path);

		// �� JSON ������Դ��Ϣ
		bool LoadFromJson(Json const& json_data);

		// �� XML �ļ�������Դ��Ϣ
		bool LoadFromXmlFile(String const& file_path);

		// �� XML �ĵ����������Դ��Ϣ
		bool LoadFromXml(tinyxml2::XMLDocument* doc);

		// ����֡ͼ��
		bool AddFrame(String const& id, String const& file_path);

		// ����֡ͼ��
		bool AddFrame(String const& id, FramePtr frame);

		// ��������֡
		UInt32 AddFrameSequence(String const& id, Vector<String> const& files);

		// ��������֡
		// ���������ü�ͼƬ
		UInt32 AddFrameSequence(String const& id, String const& file_path, Int32 cols, Int32 rows = 1);

		// ��������֡
		UInt32 AddFrameSequence(String const& id, FrameSequencePtr frames);

		// ���Ӷ���
		bool AddObjectBase(String const& id, ObjectBasePtr obj);

		// ��ȡ֡ͼ��
		FramePtr GetFrame(String const& id) const;

		// ��ȡ����֡
		FrameSequencePtr GetFrameSequence(String const& id) const;

		// ɾ��ָ����Դ
		void Delete(String const& id);

		// ���������Դ
		void Clear();

		template<typename _Ty>
		_Ty* Get(String const& id) const
		{
			auto iter = cache_.find(id);
			if (iter == cache_.end())
				return nullptr;
			return dynamic_cast<_Ty*>((*iter).second.get());
		}

	protected:
		ResourceCache();

		virtual ~ResourceCache();

	protected:
		UnorderedMap<String, ObjectBasePtr> cache_;
	};
}