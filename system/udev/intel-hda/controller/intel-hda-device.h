// Copyright 2017 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <intel-hda-driver-utils/driver-channel.h>
#include <mxtl/ref_ptr.h>

template <typename DeviceType>
class IntelHDADevice : public DriverChannel::Owner {
protected:
    friend class mxtl::RefPtr<IntelHDADevice>;
    IntelHDADevice() { }
    virtual ~IntelHDADevice() { }

    mx_status_t ProcessChannel(DriverChannel& channel,
                               const mx_io_packet_t& io_packet) final
        TA_EXCL(process_lock());

    // Exported for thread analysis purposes.
    const mxtl::Mutex& process_lock() const TA_RET_CAP(process_lock_) { return process_lock_; }

    void Shutdown() TA_EXCL(process_lock());

    ssize_t DeviceIoctl(uint32_t op,
                        const void* in_buf, size_t in_len,
                        void* out_buf, size_t out_len)
        TA_EXCL(process_lock());

private:
    // TODO(johngro) : Right now, client message processing is completely
    // serialized by the process_lock_.  If we could change this to be
    // reader/writer lock instead, we could allow multiple callbacks from
    // different channels in parallel and still be able to synchronize with all
    // callback in flight by obtaining the lock exclusively.
    mxtl::Mutex process_lock_;
    bool is_shutdown_ TA_GUARDED(process_lock_) = false;
};