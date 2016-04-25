// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fde/css/fde_csscache.h"

#include <algorithm>

#include "core/fxcrt/include/fx_ext.h"
#include "xfa/fxfa/app/xfa_textlayout.h"

FDE_CSSCacheItem::FDE_CSSCacheItem(IFDE_CSSStyleSheet* p)
    : pStylesheet(p), dwActivity(0) {
  FXSYS_assert(pStylesheet);
  pStylesheet->AddRef();
}

FDE_CSSCacheItem::~FDE_CSSCacheItem() {
  pStylesheet->Release();
}

FDE_CSSTagCache::FDE_CSSTagCache(FDE_CSSTagCache* parent,
                                 CXFA_CSSTagProvider* tag)
    : pTag(tag),
      pParent(parent),
      dwIDHash(0),
      dwTagHash(0),
      iClassIndex(0),
      dwClassHashs(1) {
  static const uint32_t s_dwIDHash = FX_HashCode_GetW(L"id", true);
  static const uint32_t s_dwClassHash = FX_HashCode_GetW(L"class", true);
  dwTagHash = FX_HashCode_GetW(pTag->GetTagName().AsStringC(), true);

  for (auto it : *pTag) {
    CFX_WideString wsValue = it.first;
    CFX_WideString wsName = it.second;
    uint32_t dwNameHash = FX_HashCode_GetW(wsName.AsStringC(), true);
    if (dwNameHash == s_dwClassHash) {
      uint32_t dwHash = FX_HashCode_GetW(wsValue.AsStringC(), false);
      dwClassHashs.Add(dwHash);
    } else if (dwNameHash == s_dwIDHash) {
      dwIDHash = FX_HashCode_GetW(wsValue.AsStringC(), false);
    }
  }
}

FDE_CSSTagCache::FDE_CSSTagCache(const FDE_CSSTagCache& it)
    : pTag(it.pTag),
      pParent(it.pParent),
      dwIDHash(it.dwIDHash),
      dwTagHash(it.dwTagHash),
      iClassIndex(0),
      dwClassHashs(1) {
  if (it.dwClassHashs.GetSize() > 0)
    dwClassHashs.Copy(it.dwClassHashs);
}

void CFDE_CSSAccelerator::OnEnterTag(CXFA_CSSTagProvider* pTag) {
  FDE_CSSTagCache* pTop = GetTopElement();
  FDE_CSSTagCache item(pTop, pTag);
  m_Stack.Push(item);
}

void CFDE_CSSAccelerator::OnLeaveTag(CXFA_CSSTagProvider* pTag) {
  FXSYS_assert(m_Stack.GetTopElement());
  FXSYS_assert(m_Stack.GetTopElement()->GetTag() == pTag);
  m_Stack.Pop();
}
