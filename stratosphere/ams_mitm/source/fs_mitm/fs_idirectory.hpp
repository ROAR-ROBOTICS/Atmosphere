/*
 * Copyright (c) 2018 Atmosphère-NX
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#include <switch.h>
#include <stratosphere.hpp>

#include "fs_results.hpp"

enum FsIDirectoryCmd : u32 {
    FsIDirectoryCmd_Read = 0,
    FsIDirectoryCmd_GetEntryCount = 1,
};

class IDirectory {
    public:
        virtual ~IDirectory() {}
        
        Result Read(uint64_t *out_count, FsDirectoryEntry *out_entries, uint64_t max_entries) {
            if (out_count == nullptr) {
                return ResultFsNullptrArgument;
            }
            if (max_entries == 0) {
                *out_count = 0;
                return 0;
            }
            if (out_entries == nullptr) {
                return ResultFsNullptrArgument;
            }
            return ReadImpl(out_count, out_entries, max_entries);
        }
        
        Result GetEntryCount(uint64_t *count) {
            if (count == nullptr) {
                return ResultFsNullptrArgument;
            }
            return GetEntryCountImpl(count);
        }
        
    protected:
        /* ...? */
    private:
        virtual Result ReadImpl(uint64_t *out_count, FsDirectoryEntry *out_entries, uint64_t max_entries) = 0;
        virtual Result GetEntryCountImpl(uint64_t *count) = 0;
};

class IDirectoryInterface : public IServiceObject {
    private:
        std::unique_ptr<IDirectory> base_dir;
    public:
        IDirectoryInterface(IDirectory *d) : base_dir(d) {
            /* ... */
        };
        IDirectoryInterface(std::unique_ptr<IDirectory> d) : base_dir(std::move(d)) {
            /* ... */
        };

    private:
        /* Actual command API. */
        virtual Result Read(OutBuffer<FsDirectoryEntry> buffer, Out<u64> out_count) final {
            return this->base_dir->Read(out_count.GetPointer(), buffer.buffer, buffer.num_elements);
        };
        virtual Result GetEntryCount(Out<u64> out_count) final {
           return this->base_dir->GetEntryCount(out_count.GetPointer());
        };
    public:
        DEFINE_SERVICE_DISPATCH_TABLE {
            /* 1.0.0- */
            MakeServiceCommandMeta<FsIDirectoryCmd_Read, &IDirectoryInterface::Read>(),
            MakeServiceCommandMeta<FsIDirectoryCmd_GetEntryCount, &IDirectoryInterface::GetEntryCount>(),
        };
};