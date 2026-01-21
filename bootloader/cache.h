/*
 * Copyright 2024 Contemporary Software
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 */

/*
 * Cache.
 */

#if !defined(CACHE_H)
#define CACHE_H

void cache_flush(void);
void cache_invalidate(void);
void cache_flush_invalidate(void);
void cache_disable_icache(void);
void cache_disable_dcache(void);
void cache_disable(void);
void cache_enable_icache(void);
void cache_enable_dcache(void);
void cache_enable(void);

#endif
