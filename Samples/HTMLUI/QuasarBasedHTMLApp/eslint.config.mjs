// ESLint flat config (ESLint 9+ / required by ESLint 10, which dropped .eslintrc).
// https://eslint.org/docs/latest/use/configure/configuration-files
//
// Named .mjs rather than .js because package.json has no "type": "module",
// so a plain .js file here would be parsed as CommonJS.

import globals from 'globals';
import tseslint from 'typescript-eslint';
import pluginVue from 'eslint-plugin-vue';
import prettierSkipFormatting from 'eslint-config-prettier';
import quasarEslint from '@quasar/app-vite/eslint';

export default tseslint.config(
  // Quasar's ignore list: dist/, .quasar/, src-capacitor/, src-cordova/,
  // and the temporary compiled quasar.config.
  ...quasarEslint.configs.recommended(),

  // .eslintignore is no longer supported by ESLint 10; the quasar helper above
  // covers dist/, .quasar/, src-capacitor/, src-cordova/ and the temp config.
  { ignores: ['src-ssr/*'] },

  // ESLint typescript rules (was 'plugin:@typescript-eslint/recommended')
  ...tseslint.configs.recommended,

  // Priority A: Essential (Error Prevention) (was 'plugin:vue/vue3-essential').
  // Swap for 'flat/strongly-recommended' or 'flat/recommended' to tighten;
  // leave only one enabled. https://eslint.vuejs.org/rules/
  ...pluginVue.configs['flat/essential'],

  {
    languageOptions: {
      globals: {
        ...globals.browser,
        ...globals.node,
        ga: 'readonly', // Google Analytics
        cordova: 'readonly',
        __statics: 'readonly',
        __QUASAR_SSR__: 'readonly',
        __QUASAR_SSR_SERVER__: 'readonly',
        __QUASAR_SSR_CLIENT__: 'readonly',
        __QUASAR_SSR_PWA__: 'readonly',
        Capacitor: 'readonly',
        chrome: 'readonly',
      },
      parserOptions: {
        // vue-eslint-parser is already the parser for .vue files (set by the
        // eslint-plugin-vue config above); this is the parser it delegates
        // <script> blocks to.
        parser: tseslint.parser,
        extraFileExtensions: ['.vue'],
      },
    },

    rules: {
      'prefer-promise-reject-errors': 'off',

      // would require an explicit return type on the `render` function
      '@typescript-eslint/explicit-function-return-type': 'off',

      // The core 'no-unused-vars' rule does not understand type definitions
      'no-unused-vars': 'off',

      // allow debugger during development only
      'no-debugger': process.env.NODE_ENV === 'production' ? 'error' : 'off',

      // ... at least for now
      '@typescript-eslint/no-unused-vars': 'off',
      '@typescript-eslint/no-inferrable-types': 'off',
      '@typescript-eslint/no-explicit-any': 'off',
      'vue/no-deprecated-slot-attribute': 'off',
      'vue/no-deprecated-v-bind-sync': 'off',
      'vue/no-deprecated-slot-scope-attribute': 'off',
      'vue/no-deprecated-filter': 'off',
      'vue/multi-word-component-names': 'off',
    },
  },

  // Keep LAST: turns off rules that conflict with Prettier's formatting.
  prettierSkipFormatting
);
