import { html } from "/core/UI.js"
import styles from "./styles.css.js"
import "/UI/components/button/index.js"

export const template = html`
    ${styles}
    <header>
        <ui-context data-key="dictionary.addresses" />
        <nav>
            <ui-icon id="add" data-icon="plus-lg" />
        </nav>
    </header>
    <div id="addresses"></div>
    <form id="address-form">
        <label>
            <input type="checkbox" name="billingAddress" />
            <ui-context data-key="dictionary.billingAddress" />
        </label>
        <label>
            <input type="checkbox" name="shippingAddress" />
            <ui-context data-key="dictionary.shippingAddress" />
        </label>
        <input type="text" name="company" />
        <input type="text" name="firstName" required />
        <input type="text" name="lastName" required />
        <label>
            <input type="radio" name="gender" value="male" required />
            <ui-context data-key="dictionary.male" />
        </label>
        <label>
            <input type="radio" name="gender" value="female" required />
            <ui-context data-key="dictionary.female" />
        </label>
        <label>
            <input type="radio" name="gender" value="other" required />
            <ui-context data-key="dictionary.other" />
        </label>
        <select name="country" required></select>
        <input type="text" name="postalCode" required />
        <input type="text" name="addressLine1" required />
        <input type="text" name="addressLine2" required />
        <input type="email" name="email" />
        <input type="tel" name="phone" required />
        <input type="hidden" name="id" />
        <div class="buttons">
            <ui-button class="full" data-left="/images/icons/house-add.svg" id="save">
                <ui-context data-key="dictionary.save"/>
            </ui-button>
            <ui-button class="full" data-left="/images/icons/x-lg.svg" id="reset">
                <ui-context data-key="dictionary.reset"/>
            </ui-button>
            <ui-button class="full" data-left="/images/icons/trash3.svg" id="remove">
                <ui-context data-key="dictionary.remove"/>
            </ui-button>
            <ui-button class="full" data-left="/images/icons/arrow-left.svg" id="back">
                <ui-context data-key="dictionary.back" />
            </ui-button>
        </div>
    </form>
`

export default template