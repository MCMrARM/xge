#pragma once

#include <exception>
#include <string>

namespace xge {

    class GLError : public std::exception {

    private:
        std::string errorStr;
        int errorCode;
        std::string throwStr;

        void setErrorDescFromCode();
        void buildErrorStr();

    public:
        /**
         * This constructor will fetch the error from the current OpenGL context
         */
        GLError();
        /**
         * This constructor will use the error code from the parameters.
         */
        GLError(int code) : errorCode(code) {
            setErrorDescFromCode();
            buildErrorStr();
        }
        /**
         * This constructor will use the error code and description from the parameters.
         */
        GLError(int code, std::string str) : errorCode(code), errorStr(str) {
            buildErrorStr();
        }

        virtual const char *what() const throw() {
            return throwStr.c_str();
        }

        inline int getCode() const {
            return errorCode;
        }

        inline std::string const &getErrorString() const {
            return throwStr;
        }

        /**
         * This function will check for OpenGL errors, and if there is any, it'll throw a GLError.
         */
        static void checkAndThrow();

    };

    class ShaderCompileError : public std::exception {

    private:
        std::string errorStr;
        std::string throwStr;

    public:
        ShaderCompileError(std::string str) : errorStr(str) {
            throwStr = "Failed to compile shader: " + str;
        }

        virtual const char *what() const throw() {
            return throwStr.c_str();
        }

        inline std::string const &getCompileLog() const {
            return errorStr;
        }

    };

    class ProgramLinkError : public std::exception {

    private:
        std::string errorStr;
        std::string throwStr;

    public:
        ProgramLinkError(std::string str) : errorStr(errorStr) {
            throwStr = "Failed to link program: " + str;
        }

        virtual const char *what() const throw() {
            return throwStr.c_str();
        }

        inline std::string const &getCompileLog() const {
            return errorStr;
        }

    };

}
