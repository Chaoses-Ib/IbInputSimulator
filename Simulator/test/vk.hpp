#pragma once
#include <Windows.h>
#include <string>

constexpr std::string vk_to_str(WORD vkCode) {
    using namespace std::literals;

    switch (vkCode) {
    case 0x01: return "VK_LBUTTON";
    case 0x02: return "VK_RBUTTON";
    case 0x03: return "VK_CANCEL";
    case 0x04: return "VK_MBUTTON";
    case 0x05: return "VK_XBUTTON1";
    case 0x06: return "VK_XBUTTON2";
    case 0x07: return "reserved";
    case 0x08: return "VK_BACK";
    case 0x09: return "VK_TAB";
    case 0x0A: return "reserved";
    case 0x0B: return "reserved";
    case 0x0C: return "VK_CLEAR";
    case 0x0D: return "VK_RETURN";
    case 0x0E: return "reserved";
    case 0x0F: return "reserved";
    case 0x10: return "VK_SHIFT";
    case 0x11: return "VK_CONTROL";
    case 0x12: return "VK_MENU";
    case 0x13: return "VK_PAUSE";
    case 0x14: return "VK_CAPITAL";
    case 0x15: return "VK_KANA|VK_HANGEUL|VK_HANGUL";
    case 0x16: return "unassigned";
    case 0x17: return "VK_JUNJA";
    case 0x18: return "VK_FINAL";
    case 0x19: return "VK_HANJA|VK_KANJI";
    case 0x1A: return "unassigned";
    case 0x1B: return "VK_ESCAPE";
    case 0x1C: return "VK_CONVERT";
    case 0x1D: return "VK_NONCONVERT";
    case 0x1E: return "VK_ACCEPT";
    case 0x1F: return "VK_MODECHANGE";
    case 0x20: return "VK_SPACE";
    case 0x21: return "VK_PRIOR";
    case 0x22: return "VK_NEXT";
    case 0x23: return "VK_END";
    case 0x24: return "VK_HOME";
    case 0x25: return "VK_LEFT";
    case 0x26: return "VK_UP";
    case 0x27: return "VK_RIGHT";
    case 0x28: return "VK_DOWN";
    case 0x29: return "VK_SELECT";
    case 0x2A: return "VK_PRINT";
    case 0x2B: return "VK_EXECUTE";
    case 0x2C: return "VK_SNAPSHOT";
    case 0x2D: return "VK_INSERT";
    case 0x2E: return "VK_DELETE";
    case 0x2F: return "VK_HELP";
    case 0x3A:
    case 0x3B:
    case 0x3C:
    case 0x3D:
    case 0x3E:
    case 0x3F:
    case 0x40: return "unassigned";
    case 0x5B: return "VK_LWIN";
    case 0x5C: return "VK_RWIN";
    case 0x5D: return "VK_APPS";
    case 0x5E: return "reserved";
    case 0x5F: return "VK_SLEEP";
    case 0x60: return "VK_NUMPAD0";
    case 0x61: return "VK_NUMPAD1";
    case 0x62: return "VK_NUMPAD2";
    case 0x63: return "VK_NUMPAD3";
    case 0x64: return "VK_NUMPAD4";
    case 0x65: return "VK_NUMPAD5";
    case 0x66: return "VK_NUMPAD6";
    case 0x67: return "VK_NUMPAD7";
    case 0x68: return "VK_NUMPAD8";
    case 0x69: return "VK_NUMPAD9";
    case 0x6A: return "VK_MULTIPLY";
    case 0x6B: return "VK_ADD";
    case 0x6C: return "VK_SEPARATOR";
    case 0x6D: return "VK_SUBTRACT";
    case 0x6E: return "VK_DECIMAL";
    case 0x6F: return "VK_DIVIDE";
    case 0x70: return "VK_F1";
    case 0x71: return "VK_F2";
    case 0x72: return "VK_F3";
    case 0x73: return "VK_F4";
    case 0x74: return "VK_F5";
    case 0x75: return "VK_F6";
    case 0x76: return "VK_F7";
    case 0x77: return "VK_F8";
    case 0x78: return "VK_F9";
    case 0x79: return "VK_F10";
    case 0x7A: return "VK_F11";
    case 0x7B: return "VK_F12";
    case 0x7C: return "VK_F13";
    case 0x7D: return "VK_F14";
    case 0x7E: return "VK_F15";
    case 0x7F: return "VK_F16";
    case 0x80: return "VK_F17";
    case 0x81: return "VK_F18";
    case 0x82: return "VK_F19";
    case 0x83: return "VK_F20";
    case 0x84: return "VK_F21";
    case 0x85: return "VK_F22";
    case 0x86: return "VK_F23";
    case 0x87: return "VK_F24";
    case 0x88: return "VK_NAVIGATION_VIEW";
    case 0x89: return "VK_NAVIGATION_MENU";
    case 0x8A: return "VK_NAVIGATION_UP";
    case 0x8B: return "VK_NAVIGATION_DOWN";
    case 0x8C: return "VK_NAVIGATION_LEFT";
    case 0x8D: return "VK_NAVIGATION_RIGHT";
    case 0x8E: return "VK_NAVIGATION_ACCEPT";
    case 0x8F: return "VK_NAVIGATION_CANCEL";
    case 0x90: return "VK_NUMLOCK";
    case 0x91: return "VK_SCROLL";
    case 0x92: return "VK_OEM_NEC_EQUAL|VK_OEM_FJ_JISHO";
    case 0x93: return "VK_OEM_FJ_MASSHOU";
    case 0x94: return "VK_OEM_FJ_TOUROKU";
    case 0x95: return "VK_OEM_FJ_LOYA";
    case 0x96: return "VK_OEM_FJ_ROYA";
    case 0x97:
    case 0x98:
    case 0x99:
    case 0x9A:
    case 0x9B:
    case 0x9C:
    case 0x9D:
    case 0x9E:
    case 0x9F: return "unassigned";
    case 0xA0: return "VK_LSHIFT";
    case 0xA1: return "VK_RSHIFT";
    case 0xA2: return "VK_LCONTROL";
    case 0xA3: return "VK_RCONTROL";
    case 0xA4: return "VK_LMENU";
    case 0xA5: return "VK_RMENU";
    case 0xA6: return "VK_BROWSER_BACK";
    case 0xA7: return "VK_BROWSER_FORWARD";
    case 0xA8: return "VK_BROWSER_REFRESH";
    case 0xA9: return "VK_BROWSER_STOP";
    case 0xAA: return "VK_BROWSER_SEARCH";
    case 0xAB: return "VK_BROWSER_FAVORITES";
    case 0xAC: return "VK_BROWSER_HOME";
    case 0xAD: return "VK_VOLUME_MUTE";
    case 0xAE: return "VK_VOLUME_DOWN";
    case 0xAF: return "VK_VOLUME_UP";
    case 0xB0: return "VK_MEDIA_NEXT_TRACK";
    case 0xB1: return "VK_MEDIA_PREV_TRACK";
    case 0xB2: return "VK_MEDIA_STOP";
    case 0xB3: return "VK_MEDIA_PLAY_PAUSE";
    case 0xB4: return "VK_LAUNCH_MAIL";
    case 0xB5: return "VK_LAUNCH_MEDIA_SELECT";
    case 0xB6: return "VK_LAUNCH_APP1";
    case 0xB7: return "VK_LAUNCH_APP2";
    case 0xB8:
    case 0xB9: return "reserved";
    case 0xBA: return "VK_OEM_1";   // ';:' for US
    case 0xBB: return "VK_OEM_PLUS";   // '+' any country
    case 0xBC: return "VK_OEM_COMMA";   // ',' any country
    case 0xBD: return "VK_OEM_MINUS";   // '-' any country
    case 0xBE: return "VK_OEM_PERIOD";   // '.' any country
    case 0xBF: return "VK_OEM_2";   // '/?' for US
    case 0xC0: return "VK_OEM_3";   // '`~' for US
    case 0xC1: return "reserved";
    case 0xC2: return "reserved";
    case 0xC3: return "VK_GAMEPAD_A"; // reserved
    case 0xC4: return "VK_GAMEPAD_B"; // reserved
    case 0xC5: return "VK_GAMEPAD_X"; // reserved
    case 0xC6: return "VK_GAMEPAD_Y"; // reserved
    case 0xC7: return "VK_GAMEPAD_RIGHT_SHOULDER"; // reserved
    case 0xC8: return "VK_GAMEPAD_LEFT_SHOULDER"; // reserved
    case 0xC9: return "VK_GAMEPAD_LEFT_TRIGGER"; // reserved
    case 0xCA: return "VK_GAMEPAD_RIGHT_TRIGGER"; // reserved
    case 0xCB: return "VK_GAMEPAD_DPAD_UP"; // reserved
    case 0xCC: return "VK_GAMEPAD_DPAD_DOWN"; // reserved
    case 0xCD: return "VK_GAMEPAD_DPAD_LEFT"; // reserved
    case 0xCE: return "VK_GAMEPAD_DPAD_RIGHT"; // reserved
    case 0xCF: return "VK_GAMEPAD_MENU"; // reserved
    case 0xD0: return "VK_GAMEPAD_VIEW"; // reserved
    case 0xD1: return "VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON"; // reserved
    case 0xD2: return "VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON"; // reserved
    case 0xD3: return "VK_GAMEPAD_LEFT_THUMBSTICK_UP"; // reserved
    case 0xD4: return "VK_GAMEPAD_LEFT_THUMBSTICK_DOWN"; // reserved
    case 0xD5: return "VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT"; // reserved
    case 0xD6: return "VK_GAMEPAD_LEFT_THUMBSTICK_LEFT"; // reserved
    case 0xD7: return "VK_GAMEPAD_RIGHT_THUMBSTICK_UP"; // reserved
    case 0xD8: return "VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN"; // reserved
    case 0xD9: return "VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT"; // reserved
    case 0xDA: return "VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT"; // reserved
    case 0xDB: return "VK_OEM_4";  //  '[{' for US
    case 0xDC: return "VK_OEM_5";  //  '\|' for US
    case 0xDD: return "VK_OEM_6";  //  ']}' for US
    case 0xDE: return "VK_OEM_7";  //  ''"' for US
    case 0xDF: return "VK_OEM_8";
    case 0xE0: return "reserved";
    case 0xE1: return "VK_OEM_AX";  //  'AX' key on Japanese AX kbd
    case 0xE2: return "VK_OEM_102";  //  "<>" or "\|" on RT 102-key kbd.
    case 0xE3: return "VK_ICO_HELP";  //  Help key on ICO
    case 0xE4: return "VK_ICO_00";  //  00 key on ICO
    case 0xE5: return "VK_PROCESSKEY";
    case 0xE6: return "VK_ICO_CLEAR";
    case 0xE7: return "VK_PACKET";
    case 0xE8: return "unassigned";
    case 0xE9: return "VK_OEM_RESET";
    case 0xEA: return "VK_OEM_JUMP";
    case 0xEB: return "VK_OEM_PA1";
    case 0xEC: return "VK_OEM_PA2";
    case 0xED: return "VK_OEM_PA3";
    case 0xEE: return "VK_OEM_WSCTRL";
    case 0xEF: return "VK_OEM_CUSEL";
    case 0xF0: return "VK_OEM_ATTN";
    case 0xF1: return "VK_OEM_FINISH";
    case 0xF2: return "VK_OEM_COPY";
    case 0xF3: return "VK_OEM_AUTO";
    case 0xF4: return "VK_OEM_ENLW";
    case 0xF5: return "VK_OEM_BACKTAB";
    case 0xF6: return "VK_ATTN";
    case 0xF7: return "VK_CRSEL";
    case 0xF8: return "VK_EXSEL";
    case 0xF9: return "VK_EREOF";
    case 0xFA: return "VK_PLAY";
    case 0xFB: return "VK_ZOOM";
    case 0xFC: return "VK_NONAME";
    case 0xFD: return "VK_PA1";
    case 0xFE: return "VK_OEM_CLEAR";
    default: if ('0' <= vkCode and vkCode <= '9'
        || 'A' <= vkCode and vkCode <= 'Z'
        ) return "VK_"s + (char)vkCode;
           else
        return std::to_string(vkCode);
    }
}

//DDCode:
/*
0 0: -1
1 VK_LBUTTON : -1
2 VK_RBUTTON : -1
3 VK_CANCEL : -1
4 VK_MBUTTON : -1
5 VK_XBUTTON1 : -1
6 VK_XBUTTON2 : -1
7 reserved : -1
8 VK_BACK : 214
9 VK_TAB : 300
10 reserved : -1
11 reserved : -1
12 VK_CLEAR : -1
13 VK_RETURN : 313
14 reserved : -1
15 reserved : -1
16 VK_SHIFT : 500
17 VK_CONTROL : 600
18 VK_MENU : 602
19 VK_PAUSE : 702
20 VK_CAPITAL : 400
21 VK_KANA | VK_HANGEUL | VK_HANGUL : -1
22 unassigned : -1
23 VK_JUNJA : -1
24 VK_FINAL : -1
25 VK_HANJA | VK_KANJI : -1
26 unassigned : -1
27 VK_ESCAPE : 100
28 VK_CONVERT : -1
29 VK_NONCONVERT : -1
30 VK_ACCEPT : -1
31 VK_MODECHANGE : -1
32 VK_SPACE : 603
33 VK_PRIOR : 705
34 VK_NEXT : 708
35 VK_END : 707
36 VK_HOME : 704
37 VK_LEFT : 710
38 VK_UP : 709
39 VK_RIGHT : 712
40 VK_DOWN : 711
41 VK_SELECT : -1
42 VK_PRINT : -1
43 VK_EXECUTE : -1
44 VK_SNAPSHOT : 700
45 VK_INSERT : 703
46 VK_DELETE : 706
47 VK_HELP : -1
48 VK_0 : 210
49 VK_1 : 201
50 VK_2 : 202
51 VK_3 : 203
52 VK_4 : 204
53 VK_5 : 205
54 VK_6 : 206
55 VK_7 : 207
56 VK_8 : 208
57 VK_9 : 209
58 unassigned : -1
59 unassigned : -1
60 unassigned : -1
61 unassigned : -1
62 unassigned : -1
63 unassigned : -1
64 unassigned : -1
65 VK_A : 401
66 VK_B : 505
67 VK_C : 503
68 VK_D : 403
69 VK_E : 303
70 VK_F : 404
71 VK_G : 405
72 VK_H : 406
73 VK_I : 308
74 VK_J : 407
75 VK_K : 408
76 VK_L : 409
77 VK_M : 507
78 VK_N : 506
79 VK_O : 309
80 VK_P : 310
81 VK_Q : 301
82 VK_R : 304
83 VK_S : 402
84 VK_T : 305
85 VK_U : 307
86 VK_V : 504
87 VK_W : 302
88 VK_X : 502
89 VK_Y : 306
90 VK_Z : 501
91 VK_LWIN : 601
92 VK_RWIN : 605
93 VK_APPS : 606
94 reserved : -1
95 VK_SLEEP : -1
96 VK_NUMPAD0 : 800
97 VK_NUMPAD1 : 801
98 VK_NUMPAD2 : 802
99 VK_NUMPAD3 : 803
100 VK_NUMPAD4 : 804
101 VK_NUMPAD5 : 805
102 VK_NUMPAD6 : 806
103 VK_NUMPAD7 : 807
104 VK_NUMPAD8 : 808
105 VK_NUMPAD9 : 809
106 VK_MULTIPLY : 812
107 VK_ADD : 814
108 VK_SEPARATOR : -1
109 VK_SUBTRACT : 813
110 VK_DECIMAL : 816
111 VK_DIVIDE : 811
112 VK_F1 : 101
113 VK_F2 : 102
114 VK_F3 : 103
115 VK_F4 : 104
116 VK_F5 : 105
117 VK_F6 : 106
118 VK_F7 : 107
119 VK_F8 : 108
120 VK_F9 : 109
121 VK_F10 : 110
122 VK_F11 : 111
123 VK_F12 : 112
124 VK_F13 : -1
125 VK_F14 : -1
126 VK_F15 : -1
127 VK_F16 : -1
128 VK_F17 : -1
129 VK_F18 : -1
130 VK_F19 : -1
131 VK_F20 : -1
132 VK_F21 : -1
133 VK_F22 : -1
134 VK_F23 : -1
135 VK_F24 : -1
136 VK_NAVIGATION_VIEW : -1
137 VK_NAVIGATION_MENU : -1
138 VK_NAVIGATION_UP : -1
139 VK_NAVIGATION_DOWN : -1
140 VK_NAVIGATION_LEFT : -1
141 VK_NAVIGATION_RIGHT : -1
142 VK_NAVIGATION_ACCEPT : -1
143 VK_NAVIGATION_CANCEL : -1
144 VK_NUMLOCK : 810
145 VK_SCROLL : 701
146 VK_OEM_NEC_EQUAL | VK_OEM_FJ_JISHO : -1
147 VK_OEM_FJ_MASSHOU : -1
148 VK_OEM_FJ_TOUROKU : -1
149 VK_OEM_FJ_LOYA : -1
150 VK_OEM_FJ_ROYA : -1
151 unassigned : -1
152 unassigned : -1
153 unassigned : -1
154 unassigned : -1
155 unassigned : -1
156 unassigned : -1
157 unassigned : -1
158 unassigned : -1
159 unassigned : -1
160 VK_LSHIFT : -1
161 VK_RSHIFT : -1
162 VK_LCONTROL : -1
163 VK_RCONTROL : -1
164 VK_LMENU : -1
165 VK_RMENU : -1
166 VK_BROWSER_BACK : -1
167 VK_BROWSER_FORWARD : -1
168 VK_BROWSER_REFRESH : -1
169 VK_BROWSER_STOP : -1
170 VK_BROWSER_SEARCH : -1
171 VK_BROWSER_FAVORITES : -1
172 VK_BROWSER_HOME : -1
173 VK_VOLUME_MUTE : -1
174 VK_VOLUME_DOWN : -1
175 VK_VOLUME_UP : -1
176 VK_MEDIA_NEXT_TRACK : -1
177 VK_MEDIA_PREV_TRACK : -1
178 VK_MEDIA_STOP : -1
179 VK_MEDIA_PLAY_PAUSE : -1
180 VK_LAUNCH_MAIL : -1
181 VK_LAUNCH_MEDIA_SELECT : -1
182 VK_LAUNCH_APP1 : -1
183 VK_LAUNCH_APP2 : -1
184 reserved : -1
185 reserved : -1
186 VK_OEM_1 : 410
187 VK_OEM_PLUS : 212
188 VK_OEM_COMMA : 508
189 VK_OEM_MINUS : 211
190 VK_OEM_PERIOD : 509
191 VK_OEM_2 : 510
192 VK_OEM_3 : 200
193 reserved : -1
194 reserved : -1
195 VK_GAMEPAD_A : -1
196 VK_GAMEPAD_B : -1
197 VK_GAMEPAD_X : -1
198 VK_GAMEPAD_Y : -1
199 VK_GAMEPAD_RIGHT_SHOULDER : -1
200 VK_GAMEPAD_LEFT_SHOULDER : -1
201 VK_GAMEPAD_LEFT_TRIGGER : -1
202 VK_GAMEPAD_RIGHT_TRIGGER : -1
203 VK_GAMEPAD_DPAD_UP : -1
204 VK_GAMEPAD_DPAD_DOWN : -1
205 VK_GAMEPAD_DPAD_LEFT : -1
206 VK_GAMEPAD_DPAD_RIGHT : -1
207 VK_GAMEPAD_MENU : -1
208 VK_GAMEPAD_VIEW : -1
209 VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON : -1
210 VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON : -1
211 VK_GAMEPAD_LEFT_THUMBSTICK_UP : -1
212 VK_GAMEPAD_LEFT_THUMBSTICK_DOWN : -1
213 VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT : -1
214 VK_GAMEPAD_LEFT_THUMBSTICK_LEFT : -1
215 VK_GAMEPAD_RIGHT_THUMBSTICK_UP : -1
216 VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN : -1
217 VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT : -1
218 VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT : -1
219 VK_OEM_4 : 311
220 VK_OEM_5 : 213
221 VK_OEM_6 : 312
222 VK_OEM_7 : 411
223 VK_OEM_8 : -1
224 reserved : -1
225 VK_OEM_AX : -1
226 VK_OEM_102 : -1
227 VK_ICO_HELP : -1
228 VK_ICO_00 : -1
229 VK_PROCESSKEY : -1
230 VK_ICO_CLEAR : -1
231 VK_PACKET : -1
232 unassigned : -1
233 VK_OEM_RESET : -1
234 VK_OEM_JUMP : -1
235 VK_OEM_PA1 : -1
236 VK_OEM_PA2 : -1
237 VK_OEM_PA3 : -1
238 VK_OEM_WSCTRL : -1
239 VK_OEM_CUSEL : -1
240 VK_OEM_ATTN : -1
241 VK_OEM_FINISH : -1
242 VK_OEM_COPY : -1
243 VK_OEM_AUTO : -1
244 VK_OEM_ENLW : -1
245 VK_OEM_BACKTAB : -1
246 VK_ATTN : -1
247 VK_CRSEL : -1
248 VK_EXSEL : -1
249 VK_EREOF : -1
250 VK_PLAY : -1
251 VK_ZOOM : -1
252 VK_NONAME : -1
253 VK_PA1 : -1
254 VK_OEM_CLEAR : -1
255 255 : -1
*/