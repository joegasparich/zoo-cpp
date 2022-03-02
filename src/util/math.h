#pragma once

float lerp(float start, float end, float percent) {
    return start + (end - start) * percent;
}

// export function lerp(start: number, end: number, percent: number): number {
//    return start + (end - start) * percent;
//}
