import React, { Component } from 'react';
import { Layout, Menu, Input } from 'antd';

export default class BasePage extends Component {
    page() { return <div></div> }

    render() {
        return <Layout>
            <Layout.Header>
                <div
                    style={{ color: '#ffffff', fontSize: '27px', float: 'left', marginRight: '28px' }}
                >
                    Gitter Kid
                </div>

                <div
                    style={{ float: 'left', marginRight: '28px' }}
                >
                    <Input
                        size="large"
                        style={{
                            height: '42px',
                            width: '360px',
                            background: '#777777',
                            color: '#ffffff',
                            borderColor: '#777777',
                            fontSize: '15px'
                        }}
                        placeholder="Search Gitter Kid"
                    />
                </div>
            </Layout.Header>
            <Layout.Content style={{ padding: '20px 50px' }}>
                <div style={{ padding: 24, minHeight: window.innerHeight - 64, background: '#ffffff' }}>
                    { this.page() }
                </div>
            </Layout.Content>
            <Layout.Footer style={{ textAlign: 'center' }}>
            </Layout.Footer>
        </Layout>
    }
}