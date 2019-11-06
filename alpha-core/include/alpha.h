// alpha.h
#pragma once
#ifndef ALPHA_H_INCLUDED
#define ALPHA_H_INCLUDED
#define PI 3.14159265358979323846f

#include <string>
#include <vector>
#include <unordered_map>
#include <cassert>

namespace alpha {
	typedef unsigned long long uint64;
	typedef   signed long long int64;
	typedef unsigned int       uint32;
	typedef   signed int       int32;
	typedef unsigned short     uint16;
	typedef   signed short     int16;
	typedef unsigned char      uint8;
	typedef   signed char      int8;

	using string = std::string;

	template <typename T>
	using dynamic_array = std::vector<T>;

	template <typename K, typename V>
	using hashmap = std::unordered_map<K, V>;

	struct time {
		static time now();

		time();
		explicit time(int64 tick);

		time operator+(const time& rhs) const;
		time operator-(const time& rhs) const;
		time& operator+=(const time& rhs);
		time& operator-=(const time& rhs);

		bool operator<(const time& rhs) const;
		bool operator>(const time& rhs) const;
		bool operator==(const time& rhs) const;
		bool operator!=(const time& rhs) const;

		float as_seconds() const;
		float as_milliseconds() const;

		int64 tick_;
	};

	struct file_system {
		static bool exists(const string& filename);
		static bool load(const string& filename, dynamic_array<uint8>& content);

		file_system();
	};

	enum keycode {
		KEYCODE_NONE = 0x00, KEYCODE_BACK = 0x08, KEYCODE_TAB = 0x09, KEYCODE_CLEAR = 0x0C,
		KEYCODE_RETURN = 0x0D, KEYCODE_SHIFT = 0x10, KEYCODE_CONTROL = 0x11, KEYCODE_MENU = 0x12,
		KEYCODE_PAUSE = 0x13, KEYCODE_CAPITAL = 0x14, KEYCODE_ESCAPE = 0x1B, KEYCODE_SPACE = 0x20,
		KEYCODE_PAGEUP = 0x21, KEYCODE_PAGEDOWN = 0x22, KEYCODE_END = 0x23, KEYCODE_HOME = 0x24,
		KEYCODE_LEFT = 0x25, KEYCODE_UP = 0x26, KEYCODE_RIGHT = 0x27, KEYCODE_DOWN = 0x28,
		KEYCODE_KEYPADENTER = 0x2B, KEYCODE_PRINTSCREEN = 0x2C, KEYCODE_INSERT = 0x2D, KEYCODE_DELETE = 0x2E,
		KEYCODE_0 = 0x30, KEYCODE_1 = 0x31, KEYCODE_2 = 0x32, KEYCODE_3 = 0x33,
		KEYCODE_4 = 0x34, KEYCODE_5 = 0x35, KEYCODE_6 = 0x36, KEYCODE_7 = 0x37,
		KEYCODE_8 = 0x38, KEYCODE_9 = 0x39, KEYCODE_A = 0x41, KEYCODE_B = 0x42,
		KEYCODE_C = 0x43, KEYCODE_D = 0x44, KEYCODE_E = 0x45, KEYCODE_F = 0x46,
		KEYCODE_G = 0x47, KEYCODE_H = 0x48, KEYCODE_I = 0x49, KEYCODE_J = 0x4A,
		KEYCODE_K = 0x4B, KEYCODE_L = 0x4C, KEYCODE_M = 0x4D, KEYCODE_N = 0x4E,
		KEYCODE_O = 0x4F, KEYCODE_P = 0x50, KEYCODE_Q = 0x51, KEYCODE_R = 0x52,
		KEYCODE_S = 0x53, KEYCODE_T = 0x54, KEYCODE_U = 0x55, KEYCODE_V = 0x56,
		KEYCODE_W = 0x57, KEYCODE_X = 0x58, KEYCODE_Y = 0x59, KEYCODE_Z = 0x5A,
		KEYCODE_LWIN = 0x5B, KEYCODE_RWIN = 0x5C, KEYCODE_KEYPAD0 = 0x60, KEYCODE_KEYPAD1 = 0x61,
		KEYCODE_KEYPAD2 = 0x62, KEYCODE_KEYPAD3 = 0x63, KEYCODE_KEYPAD4 = 0x64, KEYCODE_KEYPAD5 = 0x65,
		KEYCODE_KEYPAD6 = 0x66, KEYCODE_KEYPAD7 = 0x67, KEYCODE_KEYPAD8 = 0x68, KEYCODE_KEYPAD9 = 0x69,
		KEYCODE_MULTIPLY = 0x6A, KEYCODE_ADD = 0x6B, KEYCODE_SEPARATOR = 0x6C, KEYCODE_SUBTRACT = 0x6D,
		KEYCODE_DECIMAL = 0x6E, KEYCODE_DIVIDE = 0x6F, KEYCODE_F1 = 0x70, KEYCODE_F2 = 0x71,
		KEYCODE_F3 = 0x72, KEYCODE_F4 = 0x73, KEYCODE_F5 = 0x74, KEYCODE_F6 = 0x75,
		KEYCODE_F7 = 0x76, KEYCODE_F8 = 0x77, KEYCODE_F9 = 0x78, KEYCODE_F10 = 0x79,
		KEYCODE_F11 = 0x7A, KEYCODE_F12 = 0x7B, KEYCODE_NUMLOCK = 0x90, KEYCODE_SCROLLOCK = 0x91,
		KEYCODE_LSHIFT = 0xA0, KEYCODE_RSHIFT = 0xA1, KEYCODE_LCONTROL = 0xA2, KEYCODE_RCONTROL = 0xA3,
		KEYCODE_LMENU = 0xA4, KEYCODE_RMENU = 0xA5, KEYCODE_OEM_1 = 0xBA, KEYCODE_OEM_PLUS = 0xBB,
		KEYCODE_OEM_COMMA = 0xBC, KEYCODE_OEM_MINUS = 0xBD, KEYCODE_OEM_PERIOD = 0xBE, KEYCODE_OEM_2 = 0xBF,
		KEYCODE_OEM_3 = 0xC0, KEYCODE_OEM_4 = 0xDB, KEYCODE_OEM_5 = 0xDC, KEYCODE_OEM_6 = 0xDD,
		KEYCODE_OEM_7 = 0xDE, KEYCODE_OEM_8 = 0xDF, KEYCODE_OEM_102 = 0xE2, KEYCODE_COUNT = 256,
	};

	struct keyboard {
		keyboard();

		bool is_down(keycode index) const;
		bool is_pressed(keycode index) const;
		bool is_released(keycode index) const;

		struct keystate {
			bool down_;
			bool released_;
			bool pressed_;
		} keys_[KEYCODE_COUNT];
	};

	enum mouse_button {
		MOUSE_BUTTON_LEFT,
		MOUSE_BUTTON_RIGHT,
		MOUSE_BUTTON_MIDDLE,
		MOUSE_BUTTON_COUNT,
	};

	struct mouse {
		mouse();

		bool is_down(mouse_button index) const;
		bool is_pressed(mouse_button index) const;
		bool is_released(mouse_button index) const;

		int32 x_;
		int32 y_;
		int32 delta_;
		struct buttonstate {
			bool down_;
			bool released_;
			bool pressed_;
		} buttons_[MOUSE_BUTTON_COUNT];
	};

	struct vector2 {
		vector2();
		vector2(float x, float y);

		vector2& operator += (const vector2& v) { x_ += v.x_; y_ += v.y_; return *this; }
		vector2& operator -= (const vector2& v) { x_ -= v.x_; y_ -= v.y_; return *this; }

		bool operator == (const vector2& v) { if (x_ == v.x_ && y_ == v.y_) return true; return false; }
		bool operator != (const vector2& v) { if (x_ == v.x_ && y_ == v.y_) return false; return true; }

		const vector2 operator + (const vector2& v) const { vector2 r(*this); return r += v; }
		const vector2 operator - (const vector2& v) const { vector2 r(*this); return r -= v; }

		const vector2 operator * (const float& f) { return vector2(x_ * f, y_ * f); }
		const vector2 operator / (const float& f) { return vector2(x_ / f, y_ / f); }

		vector2 normalized() { return vector2(x_ / magnitude(), y_ / magnitude()); };
		float magnitude() { return sqrt(x_*x_ + y_*y_); };

		static vector2 Lerp(vector2 firstVector, vector2 secondVector, float lerpDistance) {
			vector2 distance = secondVector - firstVector;
			return firstVector + (distance * lerpDistance);
		}

		static vector2 zero() { return vector2(0, 0); };

		float x_;
		float y_;
	};

	struct rectangle {
		rectangle();
		rectangle(float x, float y, float width, float height);

		void set_size(const vector2 size);
		void set_position(const vector2 pos);

		float x_;
		float y_;
		float width_;
		float height_;
	};

	struct texture {
		texture();

		bool is_valid() const;
		bool create_from_file(const char* filename);
		bool create_from_memory(const int32 width, const int32 height, const void* data);
		void destroy();

		uint32 handle_;
		int32 width_;
		int32 height_;
	};

	struct sprite {
		sprite();
		explicit sprite(const uint32 color);
		explicit sprite(texture& image, vector2 size);

		void set_color(const uint32 color);
		void set_texture(const texture& image);
		void set_source(const rectangle& rect);
		void set_size(const vector2& size);

		vector2 size_;
		rectangle source_;
		uint32 color_;
		const texture* image_;
	};

	struct text {
		text();
		explicit text(const string& text, uint32 color = 0xffffffff);

		void set_color(const uint32 color);
		void set_scale(const int32 scale);
		void set_text(const char* format, ...);

		uint32 color_;
		int32 scale_;
		string text_;
	};

	struct transform {
		transform();

		void set_origin(const vector2& origin);
		void set_position(const vector2& position);
		void set_scale(const vector2& scale);
		void set_rotation(const float rotation);

		vector2 origin_;
		vector2 position_;
		vector2 scale_;
		float rotation_;
	};

	struct render_system {
		virtual ~render_system() = default;
		virtual void clear(const uint32 color = 0xff000000) = 0;
		virtual void projection(const int32 width, const int32 height) = 0;
		virtual void render(const sprite& sprite, const transform& transform) = 0;
		virtual void render(const text& text, const transform& transform) = 0;
	};

	struct audio_buffer {
		audio_buffer();

		bool is_valid() const;
		bool create_from_file(const string& filename);
		bool create_from_memory(const uint32 rate, const uint32 channels, const uint64 size, const void* data);
		void destroy();

		uint32 handle_;
		uint32 length_;
		uint32 rate_;
		uint32 channels_;
	};

	struct audio_source {
		audio_source();

		bool is_valid() const;
		bool is_playing() const;
		bool has_buffer() const;
		bool create();
		void destroy();

		uint32 handle_;
	};

	// todo: fix the api w.r.t audio_source
	struct audio_system {
		virtual ~audio_system() = default;
		virtual void process() = 0;
		virtual void play(const audio_buffer& sound, const float volume) = 0;
		virtual void play(const audio_source& source, const audio_buffer& sound, const float volume) = 0;
		virtual void pause(const audio_source& source) = 0;
		virtual void resume(const audio_source& source, const float volume) = 0;
		virtual void stop(const audio_source& source) = 0;
		virtual void set_volume(const audio_source& source, const float volume) = 0;
		virtual void stop_all() = 0;
	};

	bool network_init();
	void network_shut();

	struct network_error {
		enum code {
			NETWORK_ERROR_NO_ERROR,
			NETWORK_ERROR_INVALID_HANDLE,
			NETWORK_ERROR_NOT_ENOUGH_MEMORY,
			NETWORK_ERROR_INVALID_PARAMETER,
			NETWORK_ERROR_OPERATION_ABORTED,
			NETWORK_ERROR_INTERUPTED_CALL,
			NETWORK_ERROR_BAD_FILE_HANDLE,
			NETWORK_ERROR_SOCKET_ACCESS_DENIED,
			NETWORK_ERROR_BAD_ADDRESS,
			NETWORK_ERROR_INVALID_ARGUMENT,
			NETWORK_ERROR_TOO_MANY_OPEN_FILES,
			NETWORK_ERROR_WOULD_BLOCK,
			NETWORK_ERROR_IN_PROGRESS,
			NETWORK_ERROR_ALREADY_IN_PROGRESS,
			NETWORK_ERROR_HANDLE_NON_SOCKET,
			NETWORK_ERROR_DESTINATION_ADDRESS_REQUIRED,
			NETWORK_ERROR_MESSAGE_TOO_LONG,
			NETWORK_ERROR_WRONG_PROTOTYPE,
			NETWORK_ERROR_BAD_PROTOCOL_OPTION,
			NETWORK_ERROR_PROTOCOL_NOT_SUPPORTED,
			NETWORK_ERROR_SOCKET_TYPE_NOT_SUPPORTED,
			NETWORK_ERROR_OPERATION_NOT_SUPPORTED,
			NETWORK_ERROR_PROTOCOL_FAMILY_NOT_SUPPORT,
			NETWORK_ERROR_ADDRESS_FAMILY_NOT_SUPPORT,
			NETWORK_ERROR_ADDRESS_IN_USE,
			NETWORK_ERROR_ADDRESS_NOT_AVAILABLE,
			NETWORK_ERROR_NETWORK_DOWN,
			NETWORK_ERROR_NETWORK_UNREACHABLE,
			NETWORK_ERROR_NETWORK_DROPPED_CONNECTION,
			NETWORK_ERROR_CONNECTION_RESET_BY_SOFTWARE,
			NETWORK_ERROR_CONNECTION_RESET_BY_PEER,
			NETWORK_ERROR_NO_BUFFER_SPACE_AVAIABLE,
			NETWORK_ERROR_ALREADY_CONNECTED,
			NETWORK_ERROR_NOT_CONNECTED,
			NETWORK_ERROR_SEND_SHUTDOWN,
			NETWORK_ERROR_TOO_MANY_REFS,
			NETWORK_ERROR_CONNECTION_TIMED_OUT,
			NETWORK_ERROR_CONNECTION_REFUSED,
			NETWORK_ERROR_TRANSLATE_NAME,
			NETWORK_ERROR_NAME_TOO_LONG,
			NETWORK_ERROR_HOST_DOWN,
			NETWORK_ERROR_HOST_UNREACHABLE,
			NETWORK_ERROR_PROCESS_LIMIT,
			NETWORK_ERROR_NETWORK_SYSTEM_NOT_READY,
			NETWORK_ERROR_VERSION_NOT_SUPPORTED,
			NETWORK_ERROR_NETWORK_NOT_INITIALISED,
			NETWORK_ERROR_DISCONNECT_IN_PROGRESS,
			NETWORK_ERROR_INVALIDPROCTABLE,
			NETWORK_ERROR_INVALIDPROVIDER,
			NETWORK_ERROR_PROVIDERFAILEDINIT,
			NETWORK_ERROR_SYSCALLFAILURE,
			NETWORK_ERROR_HOST_NOT_FOUND,
			NETWORK_ERROR_TRY_AGAIN,
			NETWORK_ERROR_NO_RECOVERY,
			NETWORK_ERROR_NO_DATA,
			NETWORK_ERROR_REMOTE_DISCONNECT,
			NETWORK_ERROR_SELECT_TIMEOUT,
			NETWORK_ERROR_UNKNOWN,
		};

		static network_error get_error();

		network_error();

		bool is_critical() const;
		bool is_non_critical() const;
		const char* as_string() const;

		code code_;
	};

	namespace crypt {
		namespace generator {
			uint64 random_key();
		} // !generator

		struct xorinator {
			xorinator();
			explicit xorinator(uint64 a, uint64 b);

			bool is_valid() const;
			void encrypt(const uint64 length, uint8* data);
			void decrypt(const uint64 length, uint8* data);

			union {
				struct {
					uint64 a_;
					uint64 b_;
				} state_;
				uint8 key_[16];
			};
		};
	} // !crypt

	struct byte_stream {
		byte_stream(uint64 capacity, uint8* base);
		~byte_stream() = default;

		void reset();
		uint64 capacity() const;
		uint64 length() const;

		uint64 capacity_;
		uint8* base_;
		uint8* at_;
	};

	struct byte_stream_evaluator {
		byte_stream_evaluator(byte_stream& stream);
		~byte_stream_evaluator() = default;

		uint64 size() const;

		bool serialize(float value);
		bool serialize(uint64 value);
		bool serialize(uint32 value);
		bool serialize(uint16 value);
		bool serialize(int16 value);
		bool serialize(uint8 value);
		bool serialize(const uint32 count, const uint8* values);

		const uint64 capacity_;
		uint64 size_;
	};

	struct byte_stream_writer {
		byte_stream_writer(byte_stream& stream);
		~byte_stream_writer() = default;

		bool serialize(float value);
		bool serialize(uint64 value);
		bool serialize(uint32 value);
		bool serialize(uint16 value);
		bool serialize(int16 value);
		bool serialize(uint8 value);
		bool serialize(const uint32 count, const uint8* values);

		byte_stream& stream_;
	};

	struct byte_stream_reader {
		byte_stream_reader(byte_stream& stream);
		~byte_stream_reader() = default;

		uint8 peek() const;
		bool eos() const;

		bool serialize(float& value);
		bool serialize(uint64& value);
		bool serialize(uint32& value);
		bool serialize(uint16& value);
		bool serialize(int16& value);
		bool serialize(uint8& value);
		bool serialize(const uint32 count, uint8* values);

		byte_stream& stream_;
		uint8* cursor_;
	};

	struct ip_address {
		static bool local_addresses(dynamic_array<ip_address>& addresses);
		static bool dns_lookup(const string& dns, dynamic_array<ip_address>& addresses);

		ip_address();
		explicit ip_address(uint32 host, uint16 port = 0);
		explicit ip_address(uint8 a, uint8 b, uint8 c, uint8 d, uint16 port = 0);

		bool operator==(const ip_address& rhs) const;
		bool operator!=(const ip_address& rhs) const;

		void set_host(uint8 a, uint8 b, uint8 c, uint8 d);
		void set_port(uint16 port);
		const char* as_string() const;

		uint32 host_;
		uint16 port_;
	};

	struct udp_socket {
		udp_socket();
		~udp_socket() = default;

		bool is_valid() const;
		bool open();
		bool open(ip_address& addr);
		void close();

		bool send_to(const ip_address& address, byte_stream& stream);
		bool recv_from(ip_address& address, byte_stream& stream);
		bool address_of(ip_address& address);

		uint32 handle_;
	};

	struct application {
		static application* create(int& width, int& height, string& title);

		application();
		virtual ~application() = default;

		bool init();
		void shut();
		bool frame();

		virtual bool enter() = 0;
		virtual void exit() = 0;
		virtual bool tick() = 0;

		render_system& render_system_;
		audio_system& audio_system_;
		keyboard keyboard_;
		mouse mouse_;
	};
} // !alpha

#endif // !ALPHA_H_INCLUDED
