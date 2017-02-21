// Copyright (C) 2017 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef CPPAST_LIBCLANG_VISITOR_HPP_INCLUDED
#define CPPAST_LIBCLANG_VISITOR_HPP_INCLUDED

#include <clang-c/Index.h>

#include "raii_wrapper.hpp"

namespace cppast
{
    namespace detail
    {
        // visits direct children of an entity
        template <typename Func>
        void visit_children(CXCursor parent, Func f)
        {
            clang_visitChildren(parent,
                                [](CXCursor cur, CXCursor, CXClientData data) {
                                    auto& actual_cb = *static_cast<Func*>(data);
                                    actual_cb(cur);
                                    return CXChildVisit_Continue;
                                },
                                &f);
        }

        // visits a translation unit
        // notes: only visits if directly defined in file, not included
        template <typename Func>
        void visit_tu(const cxtranslation_unit& tu, const char* path, Func f)
        {
            auto in_tu = [&](const CXCursor& cur) {
                auto location = clang_getCursorLocation(cur);

                CXString cx_file_name;
                clang_getPresumedLocation(location, &cx_file_name, nullptr, nullptr);
                cxstring file_name(cx_file_name);

                return file_name == path;
            };

            visit_children(clang_getTranslationUnitCursor(tu.get()), [&](const CXCursor& cur) {
                if (in_tu(cur))
                    f(cur);
            });
        }
    }
} // namespace cppast::detail

#endif // CPPAST_LIBCLANG_VISITOR_HPP_INCLUDED